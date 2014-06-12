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

#include "platmisc.h"   // must precede <tk.h> otherwise Visual Studio 2013 fails to compile
#include <tk.h>
#include "inspector.h"

NAMESPACE_BEGIN


class TKENV_API GateInspector : public Inspector
{
   protected:
      char canvas[128];

   protected:
      virtual void doSetObject(cObject *obj);

   public:
      GateInspector(InspectorFactory *f);
      virtual void createWindow(const char *window, const char *geometry);
      virtual void useWindow(const char *window);
      virtual void refresh();
      virtual int redraw();
      virtual int inspectorCommand(int argc, const char **argv);

      // notifications from envir:
      virtual void displayStringChanged(cGate *gate);
};

NAMESPACE_END

#endif
