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
#include "ElevatorSimResultsWindow.hpp"
#include "SimulationState.hpp"
#include "Logger.hpp"

#include <FL/Fl.H>
#include <FL/Fl_Window.H>
#include <FL/Fl_Input.H>
#include <FL/Fl_Button.H>
#include <FL/Fl_File_Chooser.H>
#include <FL/Fl_Chart.H>
#include <FL/Enumerations.H>
#include <FL/names.h>

#include <boost/lexical_cast.hpp>
#include <fstream>

namespace elevatorSim {

/* public static member initializers */
const char ElevatorSimResultsWindow::WINDOW_TITLE[] = "Simulation Report";
const int ElevatorSimResultsWindow::WINDOW_WIDTH = 512;
const int ElevatorSimResultsWindow::WINDOW_HEIGHT = 316;
const int ElevatorSimResultsWindow::CHART_BORDER_WIDTH = 10;
const int ElevatorSimResultsWindow::CHART_BORDER_HEIGHT = 25;
const char ElevatorSimResultsWindow::CHART_TITLE[] =
   "Entrances and Exits over time";

/* public methods */
void ElevatorSimResultsWindow::updateChartData() {
   if(visible()) {
      LOG_ERROR(
         Logger::SUB_FLTK,
         "BUG: attempted to update chart data while results window was visible");
      return;
   }

   /* clear the chart data */
   resultsChart->clear();

   /* acquire a reference to the simulate state and lock it */
   SimulationState& simState = SimulationState::acquire();
   simState.lockBASM();

   /* acquire a pointer to the statistics map */
   const std::map<int, int>& entrancesAndExits =
      SimulationState::acquire().getEntrancesAndExitsReadOnly();

   /* instantiate a range variable */
   int lastEvent = 0;
   int currentPos = 0;
   for( std::map<int, int>::const_iterator ii
      = entrancesAndExits.begin(); ii != entrancesAndExits.end();
      ++ii) {

         /* retrieve values from the iterator and maybe update range variable */
         int currentIndex = ii->first;
         double currentValue = (double) ii->second;
         lastEvent = currentIndex > lastEvent ? currentIndex : lastEvent;

         /* insert the data into the new chart */
         resultsChart->insert(
            currentPos,
            currentValue,
            0,
            (currentValue < 0) ? ( FL_RED ) : ( FL_GREEN ));

         ++currentPos;
   }

   /* set the bounds */
   /* TODO: retrieve this somewhere safer */
   resultsChart->bounds( -12.0, 12.0 );
   resultsChart->autosize(1);

   simState.unlockBASM();
}

ElevatorSimResultsWindow::ElevatorSimResultsWindow() :
            Fl_Window(WINDOW_WIDTH, WINDOW_HEIGHT, WINDOW_TITLE) {

   /* instantiate the chart */
   resultsChart = new Fl_Chart(
      CHART_BORDER_WIDTH,
      CHART_BORDER_HEIGHT,
      WINDOW_WIDTH - 2 * CHART_BORDER_WIDTH,
      WINDOW_HEIGHT - 2 * CHART_BORDER_HEIGHT,
      CHART_TITLE);

   /* set the right type */
   resultsChart->type(FL_SPIKE_CHART);

   /* add it to the parent frame */
   resizable(*resultsChart);

   end();
}

int ElevatorSimResultsWindow::handle(int event) {
   if(isDebugBuild()) {
      std::stringstream dbgSS;
      dbgSS <<  fl_eventnames[event]  << std::endl;
      LOG_INFO( Logger::SUB_FLTK, sstreamToBuffer(dbgSS) );
   }

   return Fl_Window::handle(event);
}

ElevatorSimResultsWindow::~ElevatorSimResultsWindow() {
   delete resultsChart;

   if(isDebugBuild()) {
      std::stringstream dbgSS;
      dbgSS << "finished destroying results window @ " << this << std::endl;
      LOG_INFO( Logger::SUB_MEMORY, sstreamToBuffer(dbgSS) );
   }
}

} /* namespace elevatorSim */
