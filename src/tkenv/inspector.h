//==========================================================================
//  INSPECTOR.H - part of
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

#ifndef __OMNETPP_TKENV_INSPECTOR_H
#define __OMNETPP_TKENV_INSPECTOR_H

#include "tkutil.h"

namespace omnetpp {
namespace tkenv {

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
      cObject *object;        // the inspected object or nullptr if inspector is empty
      int type;               // INSP_OBJECT, etc.
      char windowName[24];    // Tk widget path   --FIXME use std::string here! (and for canvas etc)
      std::string windowTitle;// window title string
      bool isToplevelWindow;  // if so: has window title, has infobar, and destructor should destroy window
      bool closeRequested;    // "mark for deletion" flag (set if user wants to close inspector during animation)
      std::vector<cObject*> historyBack;
      std::vector<cObject*> historyForward;

   protected:
      virtual void refreshTitle();

      virtual void doSetObject(cObject *obj);
      virtual void removeFromToHistory(cObject *obj);

      void setEntry(const char *entry, const char *val);
      void setLabel(const char *label, const char *val);
      const char *getEntry(const char *entry);

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
      virtual void setObject(cObject *object);
      virtual bool canGoForward();
      virtual bool canGoBack();
      virtual void goForward();
      virtual void goBack();

      virtual void markForDeletion() {closeRequested=true;}
      virtual bool isMarkedForDeletion() {return closeRequested;}

      virtual void createWindow(const char *window, const char *geometry);
      virtual void useWindow(const char *window);
      virtual void showWindow();

      virtual void refresh();
      virtual void redraw() {refresh();}
      virtual void commit() {}

      virtual void clearObjectChangeFlags() {}

      virtual int inspectorCommand(int, const char **);

      virtual void objectDeleted(cObject *);
};

} // namespace tkenv
}  // namespace omnetpp


#endif


