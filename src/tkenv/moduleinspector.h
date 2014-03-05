//==========================================================================
//  MODULEINSPECTOR.H - part of
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

#ifndef __MODULEINSPECTOR_H
#define __MODULEINSPECTOR_H

#include <map>
#include "platmisc.h"   // must precede <tk.h> otherwise Visual Studio 2013 fails to compile
#include <tk.h>
#include "inspector.h"

NAMESPACE_BEGIN

class cModule;
class cGate;

class TKENV_API ModuleInspector : public Inspector
{
   protected:
      char canvas[128];
      bool needs_redraw;
      int32 randomSeed;
      bool notDrawn;

      struct Point {double x,y;};
      typedef std::map<cModule*,Point> PositionMap;
      PositionMap submodPosMap;  // recalculateLayout() fills this map

   protected:
      void drawSubmodule(Tcl_Interp *interp, cModule *submod, double x, double y, const char *scaling);
      void drawEnclosingModule(Tcl_Interp *interp, cModule *parentmodule, const char *scaling);
      void drawConnection(Tcl_Interp *interp, cGate *gate);

   public:
      ModuleInspector(InspectorFactory *f);
      ~ModuleInspector();
      virtual void setObject(cObject *obj);
      virtual void createWindow(const char *window, const char *geometry);
      virtual void useWindow(const char *window);
      virtual void refresh();
      virtual int inspectorCommand(Tcl_Interp *interp, int argc, const char **argv);

      bool needsRedraw() {return needs_redraw;}

      // implementations of inspector commands:
      virtual int getSubmoduleCount(Tcl_Interp *interp, int argc, const char **argv);
      virtual int getSubmodQ(Tcl_Interp *interp, int argc, const char **argv);
      virtual int getSubmodQLen(Tcl_Interp *interp, int argc, const char **argv);

      // helper for layouting code
      void getSubmoduleCoords(cModule *submod, bool& explicitcoords, bool& obeyslayout,
                                               double& x, double& y, double& sx, double& sy);

      // does full layouting, stores results in submodPosMap
      virtual void recalculateLayout();

      // updates submodPosMap (new modules, changed display strings, etc.)
      virtual void refreshLayout();

      // drawing methods:
      virtual void relayoutAndRedrawAll();
      virtual void redrawAll();

      virtual void redrawModules();
      virtual void redrawMessages();
      virtual void redrawNextEventMarker();
      virtual void updateSubmodules();

      // notifications from envir:
      virtual void submoduleCreated(cModule *newmodule);
      virtual void submoduleDeleted(cModule *module);
      virtual void connectionCreated(cGate *srcgate);
      virtual void connectionDeleted(cGate *srcgate);
      virtual void displayStringChanged();
      virtual void displayStringChanged(cModule *submodule);
      virtual void displayStringChanged(cGate *gate);
      virtual void bubble(cModule *mod, const char *text);
};

NAMESPACE_END

#endif
