//==========================================================================
//  GATEINSPECTOR.H - part of
//
//                     OMNeT++/OMNEST
//            Discrete System Simulation in C++
//
//==========================================================================

/*--------------------------------------------------------------*
  Copyright (C) 1992-2008 Andras Varga
  Copyright (C) 2006-2008 OpenSim Ltd.

  This file is distributed WITHOUT ANY WARRANTY. See the file
  `license' for details on this and other legal matters.
*--------------------------------------------------------------*/

#ifndef __GATEINSPECTOR_H
#define __GATEINSPECTOR_H

#include <map>
#include "platmisc.h"   // must precede <tk.h> otherwise Visual Studio 2013 fails to compile
#include <tk.h>
#include "inspector.h"

NAMESPACE_BEGIN


class TKENV_API GateInspector : public Inspector
{
   protected:
      char canvas[128];
   public:
      GateInspector();
      virtual void createWindow(const char *window, const char *geometry);
      virtual void refresh();
      virtual int inspectorCommand(Tcl_Interp *interp, int argc, const char **argv);

      virtual int redraw(Tcl_Interp *interp, int argc, const char **argv);

      // notifications from envir:
      virtual void displayStringChanged(cGate *gate);
};

NAMESPACE_END

#endif
