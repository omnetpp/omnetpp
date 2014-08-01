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
class cFigure;
class FigureRenderer;
class LinearCoordMapping;

enum SendAnimMode {ANIM_BEGIN, ANIM_END, ANIM_THROUGH};


class TKENV_API ModuleInspector : public Inspector
{
   public:

   protected:
      char canvas[128];
      bool needs_redraw;
      int32 layoutSeed;
      bool notDrawn;

      struct Point {double x,y;};
      typedef std::map<cModule*,Point> PositionMap;
      PositionMap submodPosMap;  // recalculateLayout() fills this map

   protected:
      void drawSubmodule(cModule *submod, double x, double y, const char *scaling);
      void drawEnclosingModule(cModule *parentmodule, const char *scaling);
      void drawConnection(cGate *gate);
      void initMapping(LinearCoordMapping& mapping);
      double getScale();
      double getZoom();
      virtual FigureRenderer *getRendererFor(cFigure *figure);
      void drawFigureRec(cFigure *figure, LinearCoordMapping& mapping);
      void refreshFigureGeometryRec(cFigure *figure, LinearCoordMapping& mapping, bool forceGeometryRefresh=false);
      void refreshFigureVisualsRec(cFigure *figure);
      static const char *animModeToStr(SendAnimMode mode);

   public:
      ModuleInspector(InspectorFactory *f);
      ~ModuleInspector();
      virtual void doSetObject(cObject *obj);
      virtual void createWindow(const char *window, const char *geometry);
      virtual void useWindow(const char *window);
      virtual void refresh();
      virtual int inspectorCommand(int argc, const char **argv);

      bool needsRedraw() {return needs_redraw;}

      // implementations of inspector commands:
      virtual int getDefaultLayoutSeed(int argc, const char **argv);
      virtual int getLayoutSeed(int argc, const char **argv);
      virtual int setLayoutSeed(int argc, const char **argv);
      virtual int getSubmoduleCount(int argc, const char **argv);
      virtual int getSubmodQ(int argc, const char **argv);
      virtual int getSubmodQLen(int argc, const char **argv);

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
      virtual void redrawFigures();
      virtual void refreshFigures();
      virtual void refreshSubmodules();

      // notifications from envir:
      virtual void submoduleCreated(cModule *newmodule);
      virtual void submoduleDeleted(cModule *module);
      virtual void connectionCreated(cGate *srcgate);
      virtual void connectionDeleted(cGate *srcgate);
      virtual void displayStringChanged();
      virtual void displayStringChanged(cModule *submodule);
      virtual void displayStringChanged(cGate *gate);
      virtual void bubble(cComponent *subcomponent, const char *text);

      virtual void animateMethodcallAscent(cModule *srcSubmodule, const char *methodText);
      virtual void animateMethodcallDescent(cModule *destSubmodule, const char *methodText);
      virtual void animateMethodcallHoriz(cModule *srcSubmodule, cModule *destSubmodule, const char *methodText);
      static void animateMethodcallDelay(Tcl_Interp *interp);
      virtual void animateMethodcallCleanup();
      virtual void animateSendOnConn(cGate *srcGate, cMessage *msg, SendAnimMode mode);
      virtual void animateSenddirectAscent(cModule *srcSubmodule, cMessage *msg);
      virtual void animateSenddirectDescent(cModule *destSubmodule, cMessage *msg, SendAnimMode mode);
      virtual void animateSenddirectHoriz(cModule *srcSubmodule, cModule *destSubmodule, cMessage *msg, SendAnimMode mode);
      virtual void animateSenddirectCleanup();
      virtual void animateSenddirectDelivery(cModule *destSubmodule, cMessage *msg);
      static void performAnimations(Tcl_Interp *interp);
};

NAMESPACE_END

#endif
