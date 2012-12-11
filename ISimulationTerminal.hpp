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

#ifndef _I_SIMULATION_TERMINAL_H
#define _I_SIMULATION_TERMINAL_H

#include "ISimulationTerminal.hpp"
#include "IStateObject.hpp"

namespace elevatorSim {

struct ISimulationTerminal : public IStateObject {
   virtual void update() = 0;

   ISimulationTerminal::ISimulationTerminal() {
      pythonRepr = NULL;
   }
   /*
    * NOTE: The dtor below is declared pure virtual but also defined in
    * the corresponding cpp file. It is pure virtual so that invocations
    * of delete on derived classes will get their own destructors invoked.
    * Even though it is pure virtual, it still must be defined because it
    * is implicitly invoked by all dtors of derived types.
    */

   virtual ~ISimulationTerminal() = 0;

   /*
    * All simulation terminals need to be able to create a tuple out of
    * themselves for processing by the python interpreter. These are the
    * functions by which this representation is manipulated, as well as
    * the pointer to the object itself.
    */
   PyObject* pythonRepr;

   PyObject* getTuple() { return pythonRepr; }

   PyObject* stealTuple() { 
      PyObject* pythonReprCopy = pythonRepr; 
      pythonRepr = NULL;
      return pythonReprCopy;
   }

   virtual void updateTuple() = 0;
   virtual void freeTuple() = 0;
};

} /* namespace elevatorSim */

#endif /* _I_SIMULATION_TERMINAL_H */
