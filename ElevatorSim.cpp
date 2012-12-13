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
#include "ElevatorSimWindow.hpp"
#include "SimulationState.hpp"
#include "Logger.hpp"

#include <boost/version.hpp>
#include <boost/thread.hpp>
#include <boost/chrono.hpp>

#include <boost/program_options.hpp>

#include <iostream>
#include <iterator>
#include <string>

using namespace elevatorSim;

void compute();
bool parseArgs(int argc, char** argv);

int main(int argc, char** argv) {
   if(!parseArgs(argc, argv)) {
      /*
       * Debug builds on windows use AllocConsole() and redirect standard
       * iostreams to it, so this ugly bullshit is required.
       */
      std::cout << "press enter to continue..." << std::endl;
      std::cin.get();
      return 1;
   }

   glutInit(&argc, argv);

   Logger::acquire();
   Py_Initialize();

   LOG_INFO(Logger::SUB_GENERAL, "logger starting up");

   srand(time(0)); /* TODO: use Boost.Random */

   SimulationState::acquire();
   ElevatorSimWindow* mainWin = new ElevatorSimWindow();
   mainWin -> show();

   boost::thread computeThread(compute);
   Fl::run();
   SimulationState::acquire().notifyKill();
   computeThread.join();

   delete mainWin;

   SimulationState::release();
   Py_Finalize();
   LOG_INFO(Logger::SUB_GENERAL, "shutting down");
   Logger::release();
   IPersonCarrier::cleanContainerCache();
   return 0;
}

void compute() {
   /* TODO: move this into a different file */
   /* TODO: make this a variable */
   static boost::chrono::milliseconds waitDuration(1);
   SimulationState& simState = SimulationState::acquire();

   while( simState.getState() != SimulationState::SIMULATION_KILLED ) {
      simState.update();
      boost::this_thread::sleep_for(waitDuration);
   }
}

bool parseArgs(int argc, char** argv) {
   using namespace boost::program_options;
   bool shouldContinue = true;

   try {
      options_description desc("Allowed options");
      desc.add_options()
                  ("help", "produce help message")
                  ("verbose", "verbose flag");

      variables_map vm;
      store(parse_command_line(argc, argv, desc), vm);
      notify(vm);

      if (vm.count("help")) {
         std::cout << desc << std::endl;
         shouldContinue = false;
      } else if (vm.count("verbose")) {
         Logger::acquire().setAllSubsystems(Logger::LOG_INFO);
      }
   } catch(std::exception& e) {
      std::cerr << "error: " << e.what() << std::endl;
      shouldContinue = false;
   } catch(...) {
      std::cerr << "Exception of unknown type" << std::endl;
      shouldContinue = false;
   }

   return shouldContinue;
}
