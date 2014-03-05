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

class InspectorFactory;

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
const char *insptypeNameFromCode(int code);
int insptypeCodeFromName(const char *namestr);


/**
 * Base class for inspectors.
 */
class TKENV_API Inspector
{
   protected:
      InspectorFactory *factory; // meta-object that describes this inspector class
      Tcl_Interp *interp;     // Tcl interpreter
      cObject *object;        // the inspected object or NULL if inspector is empty
      int type;               // INSP_OBJECT, etc.
      char windowName[24];    // Tk widget path   --FIXME use std::string here! (and for canvas etc)
      std::string windowTitle;// window title string
      bool isToplevelWindow;  // if so: has window title, has infobar, and destructor should destroy window
      bool closeRequested;    // "mark for deletion" flag (set if user wants to close inspector during animation)

   protected:
      void refreshTitle();
      void refreshInfobar();

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

      void clearInspectorListbox(const char *listbox);
      void fillInspectorListbox(const char *listbox, cObject *object, bool deep);
      void fillListboxWithSubmodules(const char *listbox, cModule *parent);

   public:
      Inspector(InspectorFactory *factory);
      virtual ~Inspector();
      virtual const char *getClassName() const;
      virtual bool supportsObject(cObject *object) const;

      static std::string makeWindowName();

      virtual int getType() const {return type;}
      virtual const char *getWindowName() const {return windowName;}
      virtual bool isToplevel() const {return isToplevelWindow;}

      virtual cObject *getObject() const {return object;}
      virtual void setObject(cObject *obj);

      virtual void hostObjectDeleted();
      virtual void markForDeletion() {closeRequested=true;}
      virtual bool isMarkedForDeletion() {return closeRequested;}

      virtual void createWindow(const char *window, const char *geometry);
      virtual void useWindow(const char *window);
      virtual void showWindow();

      virtual void refresh();
      virtual void commit() {}

      virtual int inspectorCommand(Tcl_Interp *interp, int, const char **) {return TCL_ERROR;}

      virtual void objectDeleted(cObject *) {}
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


