//==========================================================================
//  MODINSP.H -
//            for the Tcl/Tk windowing environment of
//                            OMNeT++
//==========================================================================

/*--------------------------------------------------------------*
  Copyright (C) 1992-2003 Andras Varga

  This file is distributed WITHOUT ANY WARRANTY. See the file
  `license' for details on this and other legal matters.
*--------------------------------------------------------------*/

#ifndef __MODINSP_H
#define __MODINSP_H

#include <tk.h>
#include "inspector.h"
#include "omnetapp.h"


class TModuleWindow : public TInspector
{
   //private:
   //   char modulename[64];
   //   char phase[64];
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
   public:
      TGraphicalModWindow(cObject *obj,int typ,const char *geom,void *dat=NULL);
      ~TGraphicalModWindow();
      virtual void createWindow();
      virtual void update();
      virtual int inspectorCommand(Tcl_Interp *interp, int argc, const char **argv);

      // implementations of inspector commands:
      virtual int redrawModules(Tcl_Interp *interp, int argc, const char **argv);
      virtual int getDisplayStringPar(Tcl_Interp *interp, int argc, const char **argv);
      virtual int getSubmoduleCount(Tcl_Interp *interp, int argc, const char **argv);
      virtual int getSubmodQ(Tcl_Interp *interp, int argc, const char **argv);
      virtual int getSubmodQLen(Tcl_Interp *interp, int argc, const char **argv);

      // helper methods:
      virtual void redrawMessages();
      virtual void redrawNextEventMarker();
      virtual void updateSubmodules();
      virtual void displayStringChange(cModule *, bool immediate);
};


class TCompoundModInspector: public TInspector
{
   protected:
      bool deep;
      bool simpleonly;
   public:
      TCompoundModInspector(cObject *obj,int typ,const char *geom,void *dat=NULL);
      virtual void createWindow();
      virtual void update();
      virtual void writeBack();
};


class TSimpleModInspector: public TInspector
{
   public:
      TSimpleModInspector(cObject *obj,int typ,const char *geom,void *dat=NULL);
      virtual void createWindow();
      virtual void update();
      virtual void writeBack();
};


class TGateInspector: public TInspector
{
   public:
      TGateInspector(cObject *obj,int typ,const char *geom,void *dat=NULL);
      virtual void createWindow();
      virtual void update();
};


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
};

#endif
