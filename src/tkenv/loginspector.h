//==========================================================================
//  LOGINSPECTOR.H - part of
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

#ifndef __LOGINSPECTOR_H
#define __LOGINSPECTOR_H

#include <map>
#include "platmisc.h"   // must precede <tk.h> otherwise Visual Studio 2013 fails to compile
#include <tk.h>
#include "logbuffer.h"
#include "inspector.h"

NAMESPACE_BEGIN


class TKENV_API LogInspector : public Inspector
{
   protected:
      char textWidget[128];
      std::set<int> excludedModuleIds;
   public:
      LogInspector(InspectorFactory *f);
      virtual void createWindow(const char *window, const char *geometry);
      virtual void useWindow(const char *window);
      virtual void doSetObject(cObject *obj);
      virtual void refresh();

      virtual void printLastLineOf(const LogBuffer& logBuffer);
      virtual void redisplay(const LogBuffer& logBuffer);

      virtual int inspectorCommand(Tcl_Interp *interp, int argc, const char **argv);

      static void printLastLineOf(Tcl_Interp *interp, const char *textWidget, const LogBuffer& logBuffer, const std::set<int>& excludedModuleIds);
      static void redisplay(Tcl_Interp *interp, const char *textWidget, const LogBuffer& logBuffer, cModule *mod, const std::set<int>& excludedModuleIds);
};

NAMESPACE_END


#endif
