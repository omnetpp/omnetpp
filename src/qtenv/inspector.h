//==========================================================================
//  INSPECTOR.H - part of
//
//                     OMNeT++/OMNEST
//            Discrete System Simulation in C++
//
//==========================================================================

/*--------------------------------------------------------------*
  Copyright (C) 1992-2015 Andras Varga
  Copyright (C) 2006-2015 OpenSim Ltd.

  This file is distributed WITHOUT ANY WARRANTY. See the file
  `license' for details on this and other legal matters.
*--------------------------------------------------------------*/

#ifndef __OMNETPP_QTENV_INSPECTOR_H
#define __OMNETPP_QTENV_INSPECTOR_H

#include "envir/envirbase.h"
#include "qtutil.h"
#include <QObject>

#include <QWidget>

class QContextMenuEvent;

namespace omnetpp {
namespace qtenv {

class InspectorFactory;

/**
 * Inspector types
 */
enum {
    INSP_DEFAULT,
    INSP_OBJECT,
    INSP_GRAPHICAL,
    INSP_MODULEOUTPUT,
    INSP_OBJECTTREE,
    NUM_INSPECTORTYPES   // this must be the last one
};


// utility functions
const char *insptypeNameFromCode(int code);
int insptypeCodeFromName(const char *namestr);


/**
 * Base class for inspectors.
 */
class QTENV_API Inspector : public QWidget
{
   Q_OBJECT

   protected:
      InspectorFactory *factory; // meta-object that describes this inspector class
      Tcl_Interp *interp;     // Tcl interpreterz
      cObject *object;        // the inspected object or nullptr if inspector is empty
      int type;               // INSP_OBJECT, etc.
      char windowName[24];    // Tk widget path   --FIXME use std::string here! (and for canvas etc)
      std::string windowTitle;// window title string
      bool isToplevelWindow;  // if so: has window title, has infobar, and destructor should destroy window
      std::vector<cObject*> historyBack;
      std::vector<cObject*> historyForward;

   protected:
      virtual void refreshTitle();

      virtual void doSetObject(cObject *obj);
      virtual void removeFromToHistory(cObject *obj);

      void setEntry(const char *entry, const char *val);
      void setLabel(const char *label, const char *val);
      const char *getEntry(const char *entry);

      void inspectObject(cObject *object, int type = 0, const char *geometry = "");
      void runSimulationLocal(int runMode, cObject *object = nullptr);

      void closeEvent(QCloseEvent *) override;

   public slots:
      virtual void goBack();
      virtual void goForward();
      virtual void inspectParent();

   protected slots:
      //Context menu's action's slots
      virtual void goUpInto();
      virtual void openInspector();
      virtual void openInspector(cObject *object, int type);
      virtual void runUntilModule();
      virtual void runUntilMessage();
      virtual void excludeMessage();
      virtual void utilitiesSubMenu();

   public:
      typedef QPair<OPP::cObject*, int> ActionDataPair;

      Inspector(QWidget *parent, bool isTopLevel, InspectorFactory *factory);
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

      virtual void showWindow();

      virtual void refresh();
      virtual void redraw() {refresh();}
      virtual void commit() {}

      virtual void clearObjectChangeFlags() {}

      virtual int inspectorCommand(int, const char **);

      virtual void objectDeleted(cObject *);
};

} // namespace qtenv
} // namespace omnetpp

#endif


