//==========================================================================
//  MODINSP.H - part of
//
//                     OMNeT++/OMNEST
//            Discrete System Simulation in C++
//
//==========================================================================

/*--------------------------------------------------------------*
  Copyright (C) 1992-2005 Andras Varga

  This file is distributed WITHOUT ANY WARRANTY. See the file
  `license' for details on this and other legal matters.
*--------------------------------------------------------------*/

#ifndef __MODINSP_H
#define __MODINSP_H

#ifdef _MSC_VER
#pragma warning(disable:4786)
#endif

#include <map>

#include <tk.h>

#include "csimplemodule.h"
#include "cchannel.h"
#include "cgate.h"
#include "cmessage.h"
#include "cpar.h"
#include "carray.h"
#include "coutvect.h"
#include "cstat.h"
#include "cdensity.h"
#include "cdisplaystring.h"
#include "cqueue.h"
#include "ccompoundmodule.h"
#include "cchannel.h"
#include "cbasicchannel.h"

#include "inttypes.h"
#include "inspector.h"
#include "omnetapp.h"
#include "graphlayouter.h"

class TModuleWindow : public TInspector
{
   public:
      TModuleWindow(cObject *obj,int typ,const char *geom,void *dat=NULL);
      virtual void createWindow();
      virtual void update();
};


class TGraphicalModWindow : public TInspector
{
   protected:
      char canvas[128];
      bool needs_redraw;
      int32 random_seed;
      bool not_drawn;

      struct Point {int x,y;};
      typedef std::map<cModule*,Point> PositionMap;
      PositionMap submodPosMap;  // recalculateLayout() fills this map

   public:
      TGraphicalModWindow(cObject *obj,int typ,const char *geom,void *dat=NULL);
      ~TGraphicalModWindow();
      virtual void createWindow();
      virtual void update();
      virtual int inspectorCommand(Tcl_Interp *interp, int argc, const char **argv);

      bool needsRedraw() {return needs_redraw;}

      // implementations of inspector commands:
      virtual int getDisplayStringPar(Tcl_Interp *interp, int argc, const char **argv);
      virtual int getSubmoduleCount(Tcl_Interp *interp, int argc, const char **argv);
      virtual int getSubmodQ(Tcl_Interp *interp, int argc, const char **argv);
      virtual int getSubmodQLen(Tcl_Interp *interp, int argc, const char **argv);

      // helper for layouting code
      void getSubmoduleCoords(cModule *submod, bool& explicitcoords, bool& obeyslayout,
                                               int& x, int& y, int& sx, int& sy);

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
      virtual void connectionRemoved(cGate *srcgate);
      virtual void displayStringChanged();
      virtual void displayStringChanged(cModule *submodule);
      virtual void displayStringChanged(cGate *gate);
      virtual void bubble(cModule *mod, const char *text);
};

//
// *** Note: te following inspectors have been replaced with TGenericObjectInspector ***
//
// class TCompoundModInspector: public TInspector
// {
//    protected:
//       bool deep;
//       bool simpleonly;
//    public:
//       TCompoundModInspector(cObject *obj,int typ,const char *geom,void *dat=NULL);
//       virtual void createWindow();
//       virtual void update();
//       virtual void writeBack();
// };
//
// class TSimpleModInspector: public TInspector
// {
//    public:
//       TSimpleModInspector(cObject *obj,int typ,const char *geom,void *dat=NULL);
//       virtual void createWindow();
//       virtual void update();
//       virtual void writeBack();
// };
//
// class TGateInspector: public TInspector
// {
//    public:
//       TGateInspector(cObject *obj,int typ,const char *geom,void *dat=NULL);
//       virtual void createWindow();
//       virtual void update();
//       virtual void writeBack();
// };
//

class TGraphicalGateWindow : public TInspector
{
   protected:
      char canvas[128];
   public:
      TGraphicalGateWindow(cObject *obj,int typ,const char *geom,void *dat=NULL);
      virtual void createWindow();
      virtual void update();
      virtual int inspectorCommand(Tcl_Interp *interp, int argc, const char **argv);

      virtual int redraw(Tcl_Interp *interp, int argc, const char **argv);

      // notifications from envir:
      virtual void displayStringChanged(cGate *gate);
};

class TGraphLayouterEnvironment : public GraphLayouterEnvironment
{
   protected:
      const char *canvas;
      Tcl_Interp *interp;
      cModule *parentModule;
      const cDisplayString& displayString;

   public:
      TGraphLayouterEnvironment(cModule *parentModule, const cDisplayString& displayString);

      void setCanvas(const char *canvas) { this->canvas = canvas; }
      void setInterpreter(Tcl_Interp *interp) { this->interp = interp; }

      void cleanup();

      virtual bool inspected() { return canvas && interp; }

      virtual bool getBoolParameter(const char *tagName, int index, bool defaultValue);
      virtual long getLongParameter(const char *tagName, int index, long defaultValue);
      virtual double getDoubleParameter(const char *tagName, int index, double defaultValue);

      virtual void clearGraphics();
      virtual void showGraphics(const char *text);
      virtual void drawText(int x, int y, const char *text, const char *tags, const char *color);
      virtual void drawLine(int x1, int y1, int x2, int y2, const char *tags, const char *color);
      virtual void drawRectangle(int x1, int y1, int x2, int y2, const char *tags, const char *color);
};

#endif
