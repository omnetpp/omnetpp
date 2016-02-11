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
#include <QLayout>

#include <QWidget>
#include <QDebug>

class QContextMenuEvent;
class QToolBar;

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
    INSP_MODULEOUTPUT, //TODO rename to INSP_LOG
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

      // The first setObject will set this to false.
      // Needed to perform one-off initializations.
      bool isNew = true;

   protected:
      InspectorFactory *factory; // meta-object that describes this inspector class
      cObject *object;        // the inspected object or nullptr if inspector is empty
      int type;               // INSP_OBJECT, etc.
      std::string windowTitle;// window title string
      bool isToplevelWindow;  // if so: has window title, has infobar, and destructor should destroy window
      std::vector<cObject*> historyBack;
      std::vector<cObject*> historyForward;
      QAction *goBackAction = nullptr;
      QAction *goForwardAction = nullptr;
      QAction *goUpAction = nullptr;
      QAction *findObjects = nullptr;

      static const QString PREF_GEOM;

      QString getFullPrefKey(const QString &pref, bool topLevel);
      QVariant getPref(const QString &pref, const QVariant &defaultValue);
      void setPref(const QString &pref, const QVariant &value);

      virtual void refreshTitle();

      virtual void doSetObject(cObject *obj);
      virtual void removeFromToHistory(cObject *obj);
      // override this to perform any initialization after the first setObject()
      virtual void firstObjectSet(cObject *obj);

      QSize sizeHint() const override;
      void closeEvent(QCloseEvent *) override;

      void addTopLevelToolBarActions(QToolBar *toolbar);

   public slots:
      virtual void setObject(cObject *object);
      virtual void goBack();
      virtual void goForward();
      virtual void inspectParent();
      virtual void inspectAsPopup();
      virtual void namePopup();

   protected slots:
      // Context menu's action's slot
      virtual void goUpInto();

   signals:
      void selectionChanged(cObject *object);
      void objectDoubleClicked(cObject *object);
      void inspectedObjectChanged(cObject *object);

   public:
      typedef QPair<omnetpp::cObject*, int> ActionDataPair;

      Inspector(QWidget *parent, bool isTopLevel, InspectorFactory *factory);
      virtual ~Inspector();
      virtual const char *getClassName() const;
      virtual bool supportsObject(cObject *object) const;

      virtual int getType() const {return type;}
      virtual bool isToplevel() const {return isToplevelWindow;}

      virtual cObject *getObject() const {return object;}
      virtual bool canGoForward();
      virtual bool canGoBack();

      virtual void showWindow();

      virtual void refresh();
      virtual void redraw() {refresh();}
      virtual void commit() {}

      virtual void clearObjectChangeFlags() {}

      virtual void objectDeleted(cObject *);
};

} // namespace qtenv
} // namespace omnetpp

#endif
