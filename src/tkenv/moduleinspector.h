//==========================================================================
//  MODULEINSPECTOR.H - part of
//
//                     OMNeT++/OMNEST
//            Discrete System Simulation in C++
//
//==========================================================================

/*--------------------------------------------------------------*
  Copyright (C) 1992-2017 Andras Varga
  Copyright (C) 2006-2017 OpenSim Ltd.

  This file is distributed WITHOUT ANY WARRANTY. See the file
  `license' for details on this and other legal matters.
*--------------------------------------------------------------*/

#ifndef __OMNETPP_TKENV_MODULEINSPECTOR_H
#define __OMNETPP_TKENV_MODULEINSPECTOR_H

#include <map>
#include "omnetpp/ccanvas.h"
#include "inspector.h"

namespace omnetpp {

class cModule;
class cGate;
class cFigure;

namespace tkenv {

class FigureRenderer;
struct FigureRenderingHints;
class CanvasRenderer;

enum SendAnimMode {ANIM_BEGIN, ANIM_END, ANIM_THROUGH};

class TKENV_API ModuleInspector : public Inspector
{
   public:
      struct Point {double x,y;};

   protected:
      char canvas[128];
      CanvasRenderer *canvasRenderer;

      bool needs_redraw;
      int32_t layoutSeed;
      bool notDrawn;

      typedef std::map<cModule*,Point> PositionMap;
      PositionMap submodPosMap;  // recalculateLayout() fills this map

   protected:
      cCanvas *getCanvas();
      void drawSubmodule(cModule *submod, double x, double y);
      void drawEnclosingModule(cModule *parentmodule);
      void drawConnection(cGate *gate);
      void fillFigureRenderingHints(FigureRenderingHints *hints);
      void updateBackgroundColor();
      static const char *animModeToStr(SendAnimMode mode);

   public:
      ModuleInspector(InspectorFactory *f);
      ~ModuleInspector();
      virtual void doSetObject(cObject *obj) override;
      virtual void createWindow(const char *window, const char *geometry) override;
      virtual void useWindow(const char *window) override;
      virtual void refresh() override;
      virtual void clearObjectChangeFlags() override;
      virtual int inspectorCommand(int argc, const char **argv) override;

      bool needsRedraw() {return needs_redraw;}

      Point getSubmodulePosition(const cModule *submodule);
      double getZoomLevel();

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
      virtual void redraw() override;

      virtual void redrawModules();
      virtual void redrawMessages();
      virtual void redrawNextEventMarker();
      virtual void redrawFigures();
      virtual void refreshFigures();
      virtual void refreshSubmodules();
      virtual void adjustSubmodulesZOrder();

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

} // namespace tkenv
}  // namespace omnetpp

#endif
