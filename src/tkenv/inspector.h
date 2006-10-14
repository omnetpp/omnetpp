//==========================================================================
//  INSPECTOR.H - part of
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

#ifndef __INSPECTOR_H
#define __INSPECTOR_H

#include <tk.h>
#include "tkutil.h"
#include "omnetapp.h"

//=========================================================================
// Inspector types
enum { INSP_DEFAULT,
       INSP_OBJECT,
       INSP_GRAPHICAL,
       INSP_MODULEOUTPUT,
       NUM_INSPECTORTYPES   // this must be the last one
};

const char *insptypeNameFromCode( int code );
int insptypeCodeFromName(const char *namestr);

void splitInspectorName(const char *namestr, cObject *&object, int& type);

//=========================================================================

/**
 * Virtual base class for all inspectors.
 */
class TInspector
{
   protected:
      cObject *object;        // the inspected object
      int type;               // INSP_OBJECT, etc.
      void *data;
      char windowname[24];    // Tk inspector window variable
      char windowtitle[128];  // window title string
      char geometry[64];      // X-window geometry string (window pos + size)
      bool toBeDeleted;       // "mark for deletion" flag (set if user wants to close inspector during animation)
   public:
      TInspector(cObject *obj, int typ, const char *geom, void *dat=NULL);
      virtual ~TInspector();

      virtual cObject *getObject() {return object;}
      virtual int getType() {return type;}
      virtual const char *windowName() {return windowname;}

      virtual void hostObjectDeleted();
      virtual void markForDeletion() {toBeDeleted=true;}
      virtual bool isMarkedForDeletion() {return toBeDeleted;}

      /** @name Virtual functions to be redefined in subclasses */
      //@{
      virtual void createWindow();
      bool windowExists();
      void showWindow();

      virtual void update();
      virtual void writeBack() {}

      virtual int inspectorCommand(Tcl_Interp *interp, int, const char **) {return TCL_ERROR;}

      virtual void objectDeleted(cObject *) {}
      //@}

      /** @name Utility functions */
      //@{
      void setEntry(const char *entry, const char *val);
      void setEntry(const char *entry, long l);
      void setEntry(const char *entry, double d);
      void setLabel(const char *label, const char *val);
      void setLabel(const char *label, long l);
      void setLabel(const char *label, double d);
      void setText(const char *entry, const char *val);
      void setReadonlyText(const char *entry, const char *val);
      const char *getEntry(const char *entry);
      void setInspectButton(const char *button, cObject *object, bool displayfullpath, int inspectortype);
      void setToolbarInspectButton(const char *button, cObject *object, int inspectortype);

      void deleteInspectorListbox(const char *listbox);
      void fillInspectorListbox(const char *listbox, cObject *object, bool deep);
      void fillListboxWithSubmodules(const char *listbox, cModule *parent);
      //@}
};

// this defines a panel that can be inserted into any inspector
class TInspectorPanel
{
   protected:
      char widgetname[80];
      cPolymorphic *object;
   public:
      TInspectorPanel(const char *widgetname, cPolymorphic *obj);
      virtual ~TInspectorPanel() {}
      virtual void setObject(cPolymorphic *obj);
      virtual void update() = 0;
      virtual void writeBack() = 0;
      virtual int inspectorCommand(Tcl_Interp *, int, const char **) = 0;
};

#endif


