//==========================================================================
//  TKINSP.H -
//            for the Tcl/Tk windowing environment of
//                            OMNeT++
//==========================================================================

/*--------------------------------------------------------------*
  Copyright (C) 1992-2001 Andras Varga
  Technical University of Budapest, Dept. of Telecommunications,
  Stoczek u.2, H-1111 Budapest, Hungary.

  This file is distributed WITHOUT ANY WARRANTY. See the file
  `license' for details on this and other legal matters.
*--------------------------------------------------------------*/

#ifndef __TKINSP_H
#define __TKINSP_H

#include <tk.h>

#include "omnetapp.h"

// Inspector types (1st arg to cXXXX::inspector(int,void *) functions)
enum { INSP_GETTYPES=-1,
       INSP_DEFAULT,
       INSP_OBJECT,
       INSP_GRAPHICAL,
       INSP_CONTAINER,
       INSP_MODULEOUTPUT,
       INSP_LOCALVARS,
       INSP_CLASSMEMBERS,
       INSP_PARAMETERS,
       INSP_GATES,
       INSP_SUBMODS,
       NUM_INSPECTORTYPES   // this must be the last one
};

char *insptype_name_from_code( int code );
int insptype_code_from_name( char *namestr );

//=========================================================================
// virtual base class for all inspectors

class TInspector : public cObject
{
   public:
      cObject *object;        // the inspected object
      int type;               // INSP_NORMAL, etc.
      void *data;
      char windowname[24];    // Tk inspector window variable
      char windowtitle[128];  // window title string
   public:
      TInspector(cObject *obj, int typ, void *dat=NULL);
      ~TInspector();

      // virtual functions to be redefined in descendants:
      virtual void createWindow();
      bool windowExists();
      void showWindow();

      virtual void update();
      virtual void writeBack() {}

      virtual int inspectorCommand(Tcl_Interp *, int, char **) {return TCL_ERROR;}

      // utility functions:
      void setEntry(const char *entry, const char *val);
      void setEntry(const char *entry, long l);
      void setEntry(const char *entry, double d);
      void setLabel(const char *label, const char *val);
      void setLabel(const char *label, long l);
      void setLabel(const char *label, double d);
      const char *getEntry(const char *entry);
      void setInspectButton(const char *button, cObject *object, int type);
      void setButtonText(const char *button, const char *text);
      void deleteInspectorListbox(const char *listbox);
      void fillInspectorListbox(const char *listbox, cObject *object,
                                InfoFunc infofunc,bool deep);
      void fillModuleListbox(const char *listbox, cModule *parent,
                             InfoFunc infofunc, bool simpleonly, bool deep);

};

//=========================================================================
// inspectors

class TObjInspector : public TInspector
{
   public:
      TObjInspector(cObject *obj,int typ,void *dat=NULL);
      virtual void createWindow();
      virtual void update();
};

class TContainerInspector : public TInspector
{
   public:
      InfoFunc infofunc;
      bool deep;
      char listbox[64];
   public:
      TContainerInspector(cObject *obj,int typ,void *dat=NULL,
                          InfoFunc f=NULL);
      virtual void createWindow();
      virtual void update();
};

class TModuleWindow : public TInspector
{
   private:
      char modulename[64];
      char phase[64];
   public:
      TModuleWindow(cObject *obj,int typ,void *dat=NULL);
      virtual void createWindow();
      virtual void update();
};

class THistogramWindow : public TInspector
{
   public:
      char canvas[64];
   public:
      THistogramWindow(cObject *obj,int typ,void *dat=NULL);
      virtual void createWindow();
      virtual void update();
      virtual void writeBack() {}
      virtual int inspectorCommand(Tcl_Interp *interp, int argc, char **argv);

      // return textual information in general or about a value/value pair
      void generalInfo( char *buf );
      void cellInfo( char *buf, int cellindex );
};

class CircBuffer
{
   public:
      struct CBEntry {
        simtime_t t;
        double value1, value2;
      };
      CBEntry *buf;
      int size, n, head;
   CircBuffer( int size );
   ~CircBuffer();
   void add(simtime_t t, double value1, double value2);
   int tail() {return (head-n+1+size)%size;}
};

class TOutVectorWindow : public TInspector
{
   public:
      char canvas[64];        // widget namestr
      CircBuffer circbuf;    // buffer to store values

      // configuration
      enum {DRAW_DOTS, DRAW_BARS, DRAW_PINS, DRAW_SAMPLEHOLD, DRAW_LINES, NUM_DRAWINGMODES};
      bool autoscale;
      int drawing_mode;
      double miny, maxy;
      double time_factor; // x scaling: secs/10 pixel

   public:
      TOutVectorWindow(cObject *obj,int typ,void *dat=NULL, int size=100);
      ~TOutVectorWindow();
      virtual void createWindow();
      virtual void update();
      virtual int inspectorCommand(Tcl_Interp *interp, int argc, char **argv);

      // return textual information in general or about a value/value pair
      void generalInfo( char *buf );
      void valueInfo( char *buf, int valueindex );

      // configuration get (text form) / set
      void getConfig( char *buf );
      void setConfig( double timef, double miny, double maxy, char *mode );
};

class TMessageInspector: public TInspector
{
   public:
      TMessageInspector(cObject *obj,int typ,void *dat=NULL);
      virtual void createWindow();
      virtual void update();
      virtual void writeBack();
};

class TWatchInspector: public TInspector
{
   public:
      TWatchInspector(cObject *obj,int typ,void *dat=NULL);
      virtual void createWindow();
      virtual void update();
      virtual void writeBack();
};

class TStatisticInspector: public TInspector
{
   public:
      TStatisticInspector(cObject *obj,int typ,void *dat=NULL);
      virtual void createWindow();
      virtual void update();
};

class TParInspector: public TInspector
{
   public:
      TParInspector(cObject *obj,int typ,void *dat=NULL);
      virtual void createWindow();
      virtual void update();
      virtual void writeBack();
};

class TGraphicalModWindow : public TInspector
{
   public:
      char canvas[128];
      bool needs_redraw;
   public:
      TGraphicalModWindow(cObject *obj,int typ,void *dat=NULL);
      ~TGraphicalModWindow();
      virtual void createWindow();
      virtual void update();
      virtual int inspectorCommand(Tcl_Interp *interp, int argc, char **argv);

      virtual int redrawModules(Tcl_Interp *interp, int argc, char **argv);
      virtual int redrawMessages(Tcl_Interp *interp, int argc, char **argv);
      virtual int getDisplayStringPar(Tcl_Interp *interp, int argc, char **argv);
      virtual void displayStringChange(cModule *, bool immediate);

};

class TCompoundModInspector: public TInspector
{
   public:
      bool deep;
      bool simpleonly;
   public:
      TCompoundModInspector(cObject *obj,int typ,void *dat=NULL);
      virtual void createWindow();
      virtual void update();
};

class TSimpleModInspector: public TInspector
{
   public:
      TSimpleModInspector(cObject *obj,int typ,void *dat=NULL);
      virtual void createWindow();
      virtual void update();
};

class TGateInspector: public TInspector
{
   public:
      TGateInspector(cObject *obj,int typ,void *dat=NULL);
      virtual void createWindow();
      virtual void update();
};

class TGraphicalGateWindow : public TInspector
{
   public:
      char canvas[128];
   public:
      TGraphicalGateWindow(cObject *obj,int typ,void *dat=NULL);
      virtual void createWindow();
      virtual void update();
      virtual int inspectorCommand(Tcl_Interp *interp, int argc, char **argv);

      virtual int redraw(Tcl_Interp *interp, int argc, char **argv);
};

class TPacketInspector: public TMessageInspector
{
   public:
      TPacketInspector(cObject *obj,int typ,void *dat=NULL);
      virtual void createWindow();
      virtual void update();
      virtual void writeBack();
};

#endif
