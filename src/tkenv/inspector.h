//==========================================================================
//  INSPECTOR.H - part of
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

#ifndef __INSPECTOR_H
#define __INSPECTOR_H

#include <tk.h>
#include "tkutil.h"
#include "envirbase.h"

NAMESPACE_BEGIN

/**
 * Inspector types
 */
enum {
    INSP_DEFAULT,
    INSP_OBJECT,
    INSP_GRAPHICAL,
    INSP_MODULEOUTPUT,
    NUM_INSPECTORTYPES   // this must be the last one
};


// utility functions
const char *insptypeNameFromCode( int code );
int insptypeCodeFromName(const char *namestr);


/**
 * Virtual base class for all inspectors.
 */
class TKENV_API Inspector
{
   protected:
      Tcl_Interp *interp;     // Tcl interpreter
      cObject *object;        // the inspected object or NULL if inspector is empty
      int type;               // INSP_OBJECT, etc.
      char windowName[24];    // Tk widget path
      std::string windowTitle;// window title string
      bool ownsWindow;        // whether destructor should destroy the window
      bool closeRequested;    // "mark for deletion" flag (set if user wants to close inspector during animation)
   public:
      Inspector();
      virtual ~Inspector();

      virtual cObject *getObject() {return object;}
      virtual void setObject(cObject *obj) {object = obj;} // obj=NULL should be accepted; override to refresh GUI accordingly
      virtual int getType() {return type;}
      virtual void setType(int t) {type = t;}
      virtual const char *getWindowName() {return windowName;}

      virtual void hostObjectDeleted();
      virtual void markForDeletion() {closeRequested=true;}
      virtual bool isMarkedForDeletion() {return closeRequested;}

      /** @name Virtual functions to be redefined in subclasses */
      //@{
      virtual void createWindow(const char *window, const char *geometry);
      virtual void useWindow(const char *widget);
      virtual bool windowExists();
      virtual void showWindow();

      virtual void refresh();
      virtual void commit() {}

      virtual int inspectorCommand(Tcl_Interp *interp, int, const char **) {return TCL_ERROR;}

      virtual void objectDeleted(cObject *) {}
      //@}

      /** @name Utility functions */
      //@{
      static std::string makeWindowName();

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

/**
 * Defines a panel that can be inserted into any inspector
 */
class TKENV_API GenericObjectViewer
{
   protected:
      char widgetname[80];
      cObject *object;
   public:
      GenericObjectViewer(const char *widgetname, cObject *obj);
      virtual ~GenericObjectViewer() {}
      virtual void setObject(cObject *obj);
      virtual void update() = 0;
      virtual void writeBack() = 0;
      virtual int inspectorCommand(Tcl_Interp *, int, const char **) = 0;
};

NAMESPACE_END


#endif


