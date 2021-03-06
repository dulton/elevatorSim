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

#ifndef _ELEVATOR_SIM_WELCOME_WINDOW_H
#define _ELEVATOR_SIM_WELCOME_WINDOW_H

#include "ElevatorSim.hpp"
#include "cTimeManager.hpp"
#include "cKeyManager.hpp"

#include <FL/Fl.H>
#include <FL/Fl_Window.H>
#include <FL/Fl_Button.H>
#include <FL/Fl_Menu_Bar.H>
#include <FL/Fl_File_Chooser.H>
#include <FL/Fl_Text_Display.H>
#include <FL/Fl_Check_Button.H>

namespace elevatorSim
{
class ElevatorSimWelcomeWindow : public Fl_Window {
private:

   int handle(int event);
   void writeDatFile();
   void readDatFile();
   void selectTipFromFile();

   /* private static constants */
   const static char FIRST_RUN_FILENAME[];
   const static char TIPS_CATALOG_FILENAME[];

   /* private members */
   bool showTips;
   bool hidden;

   /* quit confirmation widgets */
   Fl_Text_Buffer* welcomeTextBuffer;
   Fl_Text_Display* welcomeDisplay;
   Fl_Check_Button* checkButton;

public:

   /* public static members */
   const static int WINDOW_WIDTH;
   const static int WINDOW_HEIGHT;
   const static char WINDOW_TITLE[];

   inline bool shouldShowTips() const {
      return !hidden && showTips;
   }

   ElevatorSimWelcomeWindow();
   ~ElevatorSimWelcomeWindow();
};

} /* namespace elevatorSim */

#endif /* _ELEVATOR_SIM_WELCOME_WINDOW_H */
