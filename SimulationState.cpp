/*
 * Copyright (c) 2012, Joseph Max DeLiso
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are met:
 *
 * 1. Redistributions of source code must retain the above copyright notice,
 * this list of conditions and the following disclaimer.
 *
 * 2. Redistributions in binary form must reproduce the above copyright notice,
 * this list of conditions and the following disclaimer in the documentation
 * and/or other materials provided with the distribution.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
 * AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 * ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT OWNER OR CONTRIBUTORS BE
 * LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
 * CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
 * SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
 * INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
 * CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
 * ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
 * POSSIBILITY OF SUCH DAMAGE.
 *
 * The views and conclusions contained in the software and documentation are
 * those of the authors and should not be interpreted as representing official
 * policies, either expressed or implied, of the FreeBSD Project.
 */

#include "ElevatorSim.hpp"
#include "SimulationState.hpp"
#include "cTimeManager.hpp"
#include "cKeyManager.hpp"
#include "cRenderObjs.hpp"
#include "cCameraManager.hpp"
#include "Building.hpp"
#include "Logger.hpp"

#include <boost/thread/mutex.hpp>
#include <set>
#include <functional>
#include <cassert>
#include <sstream>
#include <string>
#include <fstream>

namespace elevatorSim   {

SimulationState* SimulationState::simulationState = NULL;

void SimulationState::saveEntranceUnsafe( const int numPeople ) {
   entrancesAndExits.push_back(
      std::pair<int,int> (logicTicks, numPeople));
}

void SimulationState::saveExitUnsafe( const int numPeople) {
   entrancesAndExits.push_back(
      std::pair<int,int> (logicTicks, -numPeople));
}


SimulationState::SimulationState() : cState(SimulationState::SIMULATION_STARTING) {
   stateObjects.insert((timeManager = new cTimeManager()));
   stateObjects.insert((keyManager = new cKeyManager()));
   stateObjects.insert((cameraManager = new cCameraManager()));
   stateObjects.insert((building = new Building()));

   userScriptCodeObject = NULL;
   userScriptExecModule = NULL;
   userComputeFunction = NULL;

   renderObjs = NULL;
   logicTicks = 0;
}

SimulationState::~SimulationState() {
   if(isDebugBuild()) {
      std::stringstream dbgSS;
      dbgSS << "starting to destroy simulation state @ " << this << std::endl;
      LOG_INFO( Logger::SUB_MEMORY, sstreamToBuffer(dbgSS) );
   }

   init();

   delete building;

   delete cameraManager;
   delete renderObjs;
   delete keyManager;
   delete timeManager;

   if(isDebugBuild()) {
      std::stringstream dbgSS;
      dbgSS << "finished destroying simulation state @ " << this << std::endl;
      LOG_INFO( Logger::SUB_MEMORY, sstreamToBuffer(dbgSS) );
   }
}

SimulationState& SimulationState::acquire() {
   if(simulationState == NULL) {
      simulationState = new SimulationState();
      simulationState -> init();
   }

   return *simulationState;
}

void SimulationState::release() {
   assert(simulationState != NULL);
   delete simulationState;
   simulationState = NULL;
}

void SimulationState::init() {
   cState = SIMULATION_STARTING;
   logicTicks = 0;

   Py_CLEAR(userScriptCodeObject);
   Py_CLEAR(userScriptExecModule);
   Py_CLEAR(userComputeFunction);
}

void SimulationState::update() {
   bigAssStateMutex.lock();

   if( cState == SIMULATION_RUNNING ) {
      std::for_each(
         stateObjects.begin(),
         stateObjects.end(),
         [] (IStateObject * stateObj) {
            stateObj -> update();
      });

      ++logicTicks;
   }

   bigAssStateMutex.unlock();
}

void SimulationState::start(
   int numElevators,
   int numFloors,
   int randomSeed,
   const std::string& pyAiPath ) {
      assert(
         userScriptCodeObject == NULL && 
         userScriptExecModule == NULL &&
         userComputeFunction == NULL);

      bigAssStateMutex.lock();

      /* cRenderObjs */

      init();

      /* load and compile python */
      if( loadPythonScript( pyAiPath ) ) {

         timeManager->init();
         keyManager->init();
         cameraManager->init();
         entrancesAndExits.clear();

         stateObjects.erase(building);
         delete building;
         building = new Building(numFloors, numElevators);
         stateObjects.insert(building);

         srand(randomSeed);

         cState = SIMULATION_RUNNING;
      }

      bigAssStateMutex.unlock();
}

void SimulationState::runUserScriptUnsafe() {
   assert( userComputeFunction != NULL );

   building->updateTuple();
   PyObject* computeFunctionArgs = building->getTuple();
   
   if(isDebugBuild()) {
      LOG_INFO( Logger::SUB_ELEVATOR_LOGIC, "invoking python on user script..." );
   }

   PyObject* computeFunctionResult = 
      PyObject_CallObject(
         userComputeFunction,
         computeFunctionArgs);

   if(isDebugBuild()) {
      std::stringstream dbgSS;
      dbgSS << "created object at: " << computeFunctionResult << std::endl;
      LOG_INFO( Logger::SUB_MEMORY, sstreamToBuffer(dbgSS) );
   }

   if(computeFunctionResult == NULL || PyErr_Occurred()) {
      PyErr_Print();

      if(isDebugBuild()) {
         LOG_ERROR( Logger::SUB_ELEVATOR_LOGIC, 
            "error occurred while attempting to call python object!" );
      }

   } else {
      if(isDebugBuild()) {
         LOG_INFO( Logger::SUB_ELEVATOR_LOGIC, 
            "got valid result from python. parsing..." );
      }

      int numSignals = PyTuple_Size(computeFunctionResult);

      for( int i = 0; i < numSignals; ++i ) {
         PyObject* signalTuple = PyTuple_GetItem(computeFunctionResult, i);

         if( !PyTuple_Check(signalTuple) || PyTuple_Size( signalTuple ) != 2 ) {
            LOG_ERROR( Logger::SUB_ELEVATOR_LOGIC,
               "got invalid signal tuple. discarding...");
         } else {
            PyObject* elevNumObj = PyTuple_GetItem(signalTuple, 0);
            PyObject* elevDestObj = PyTuple_GetItem(signalTuple, 1);

            if(elevNumObj == NULL || elevDestObj == NULL ) {
               PyErr_Print();

               LOG_ERROR( Logger::SUB_ELEVATOR_LOGIC,
                  "couldn't parse signal tuple to a pair of ints. check python code.");
            } else {

               int elevNum = PyLong_AsLong(elevNumObj);
               int elevDest = PyLong_AsLong(elevDestObj);

               if(isDebugBuild()) {
                  std::stringstream dbgSS;

                  dbgSS << "signal pair from python: " << elevNum
                     << " " << elevDest << std::endl;

                  LOG_INFO( Logger::SUB_ELEVATOR_LOGIC,
                     sstreamToBuffer(dbgSS) );
               }

               dispatchElevatorToFloor( 
                  elevNum, 
                  elevDest);
            }
         } /* else signal tuple is valid */
      } /* for each signal */
   } /* if computeFunction result was invalid */

   if(isDebugBuild()) {
      std::stringstream dbgSS;
      dbgSS << "XDECREF on " << computeFunctionResult << std::endl;
      LOG_INFO( Logger::SUB_MEMORY, sstreamToBuffer(dbgSS) );
   }

   Py_XDECREF(computeFunctionResult);
}

void SimulationState::dispatchElevatorToFloor(
   const unsigned int elev,
   const unsigned int floor ) {
      if( elev >= building->getElevators().size()) {
         LOG_ERROR( Logger::SUB_ELEVATOR_LOGIC,
            "elevator index dispatched python out of range. ignoring..." );
         return;
      }

      if( floor >= building->getFloors().size()) {
         LOG_ERROR( Logger::SUB_ELEVATOR_LOGIC,
            "floor index dispatched python out of range. ignoring..." );
         return;
      }

      building->getElevators()[elev]->goToFloor(floor);
}

bool SimulationState::togglePause() {
   bool ret = false;
   bigAssStateMutex.lock();

   if( cState != SIMULATION_RUNNING && cState != SIMULATION_PAUSED ) {
      ret = false;      
   } else {

      if( cState == SIMULATION_PAUSED ) {
         ret = false;
         cState = SIMULATION_RUNNING;
      } else {
         ret = true;
         cState = SIMULATION_PAUSED;
      }
   }

   bigAssStateMutex.unlock();
   return ret;
}

void SimulationState::stop() {
   bigAssStateMutex.lock();
   init();
   cState = SIMULATION_READY;

   /* TODO: post-process simulation data */

   bigAssStateMutex.unlock();
}

const char SimulationState::USER_SCRIPT_PY_NAME[] = "elevatorSim";
const char SimulationState::USER_SCRIPT_PY_FUNC_NAME[] = "computeNextMove";

PyObject* SimulationState::simStateToTuple() {


   return NULL;
}

void SimulationState::decrefSimStateTuple(PyObject* simStateTuple) {
   (void) simStateTuple;
}

bool SimulationState::loadPythonScript( const std::string& pyAiPath ) {
   std::string pyBuffer;
   std::ifstream pyScriptFile( pyAiPath.c_str(), std::ifstream::in );

   if (!pyScriptFile.is_open()) {
      LOG_ERROR( Logger::SUB_GENERAL, "couldn't open script file");
      return false;
   }

   while ( pyScriptFile.good() ) {
      std::string lineBuffer;
      getline (pyScriptFile, lineBuffer);
      pyBuffer += (lineBuffer + '\n');
   }

   pyScriptFile.close();

   /* compile the string into a code object */
   userScriptCodeObject =
      Py_CompileString(
         pyBuffer.c_str(),
         pyAiPath.c_str(),
         Py_file_input);

   if( userScriptCodeObject == NULL ) {
      PyErr_Print();
      return false;
   }
   
   /*
    * std::cerr << "Code object: ";
    * PyObject_Print( userScriptCodeObject, stderr, 0);
    * std::cerr << std::endl;
    */

   userScriptExecModule = PyImport_ExecCodeModule(
      (char*) USER_SCRIPT_PY_NAME, userScriptCodeObject );

   if( userScriptExecModule == NULL ) {
      PyErr_Print();
      Py_CLEAR(userScriptCodeObject);
      return false;
   }

   /*
    * std::cerr << "Executable module: ";
    * PyObject_Print( userScriptExecModule, stderr, 0);
    * std::cerr << std::endl;
    */

   userComputeFunction = 
      PyObject_GetAttrString( 
         userScriptExecModule, 
         USER_SCRIPT_PY_FUNC_NAME );

   if( userComputeFunction == NULL || 
      !PyCallable_Check(userComputeFunction)) {
         PyErr_Print();
         Py_CLEAR(userScriptExecModule);
         Py_CLEAR(userScriptCodeObject);
         Py_CLEAR(userComputeFunction);
         return false;
   }  

   return true;
} 

} /* namespace elevatorSim */
