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
#include "inspector.h"
#include "omnetapp.h"


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
      int random_seed;
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
      virtual void displayStringChanged(cModule *submodule);
      virtual void backgroundDisplayStringChanged();
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

#endif
