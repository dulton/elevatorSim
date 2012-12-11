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

#ifndef _ELEVATOR_SIM_START_WINDOW_H
#define _ELEVATOR_SIM_START_WINDOW_H

#include "ElevatorSim.hpp"

#include <FL/Fl.H>
#include <FL/Fl_Window.H>
#include <FL/Fl_Input.H>
#include <FL/Fl_Button.H>
#include <FL/Fl_File_Chooser.H>

#include <string>

namespace elevatorSim {
class ElevatorSimStartWindow : public Fl_Window {

   /* private methods */
   int handle(int event);

   static bool validateSimulationParams(
            const int numElev,
            const int numFloor,
            const int rSeed,
            const std::string& pyAiPath);

   /* fltk callbacks */
   static void browseCB(Fl_Button* b, void* userData);
   static void inputAcceptCB(Fl_Window*  w, void* userData);
   static void inputCancelCB(Fl_Window* w, void* userData);
   static void fileChosenCB(Fl_File_Chooser* fc, void* userData);

   /* user input widget */
   Fl_Input* elevatorNumInput;
   Fl_Input* floorNumInput;
   Fl_Input* seedNumInput;
   Fl_Input* elevatorAIPathInput;
   Fl_File_Chooser* elevatorAIFileChooser;
   Fl_Button *browseButton;
   Fl_Button *inputAccept;
   Fl_Button *inputCancel;

public:

   /* public static members */
   const static int WINDOW_WIDTH;
   const static int WINDOW_HEIGHT;
   const static char WINDOW_TITLE[];

   int getElevatorNum();
   int getFloorNum();
   int getSeedNum();

   ElevatorSimStartWindow();
   ~ElevatorSimStartWindow();
};

} /* namespace elevatorSim */

#endif /* _ELEVATOR_SIM_START_WINDOW_H */
