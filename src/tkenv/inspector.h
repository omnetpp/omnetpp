//==========================================================================
//  INSPECTOR.H -
//            for the Tcl/Tk windowing environment of
//                            OMNeT++
//==========================================================================

/*--------------------------------------------------------------*
  Copyright (C) 1992-2003 Andras Varga

  This file is distributed WITHOUT ANY WARRANTY. See the file
  `license' for details on this and other legal matters.
*--------------------------------------------------------------*/

#ifndef __INSPECTOR_H
#define __INSPECTOR_H

#include <tk.h>
#include "omnetapp.h"

//=========================================================================
// Inspector types
enum { INSP_DEFAULT,
       INSP_OBJECT,
       INSP_GRAPHICAL,
       INSP_MODULEOUTPUT,
       NUM_INSPECTORTYPES   // this must be the last one
};

const char *insptype_name_from_code( int code );
int insptype_code_from_name(const char *namestr);

//=========================================================================

/**
 * Virtual base class for all inspectors.
 */
class TInspector : public cObject
{
   protected:
      cObject *object;        // the inspected object
      int type;               // INSP_OBJECT, etc.
      void *data;
      char windowname[24];    // Tk inspector window variable
      char windowtitle[128];  // window title string
      char geometry[64];      // X-window geometry string (window pos + size)
   public:
      TInspector(cObject *obj, int typ, const char *geom, void *dat=NULL);
      virtual ~TInspector();

      virtual cObject *getObject() {return object;}
      virtual int getType() {return type;}
      virtual const char *windowName() {return windowname;}

      virtual void hostObjectDeleted();

      // virtual functions to be redefined in descendants:
      virtual void createWindow();
      bool windowExists();
      void showWindow();

      virtual void update();
      virtual void writeBack() {}

      virtual int inspectorCommand(Tcl_Interp *interp, int, const char **) {return TCL_ERROR;}

      // utility functions:
      void setEntry(const char *entry, const char *val);
      void setEntry(const char *entry, long l);
      void setEntry(const char *entry, double d);
      void setLabel(const char *label, const char *val);
      void setLabel(const char *label, long l);
      void setLabel(const char *label, double d);
      const char *getEntry(const char *entry);
      void setInspectButton(const char *button, cObject *object, bool displayfullpath, int inspectortype);
      void setToolbarInspectButton(const char *button, cObject *object, int inspectortype);

      void deleteInspectorListbox(const char *listbox);
      void fillInspectorListbox(const char *listbox, cObject *object,
                                InfoFunc infofunc,bool deep);
      void fillModuleListbox(const char *listbox, cModule *parent,
                             InfoFunc infofunc, bool simpleonly, bool deep);

};

// this defines a panel that can be inserted into any inspector
class TInspectorPanel
{
   protected:
      char widgetname[80];
      cObject *object;
   public:
      TInspectorPanel(const char *widgetname, cObject *obj);
      virtual void update() = 0;
      virtual void writeBack() = 0;
      virtual int inspectorCommand(Tcl_Interp *, int, const char **) = 0;
};

#endif


