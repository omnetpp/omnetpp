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

#ifndef __OMNETPP_QTENV_INSPECTOR_H
#define __OMNETPP_QTENV_INSPECTOR_H

#include "envir/envirbase.h"
#include "inspectorutiltypes.h"
#include "qtutil.h"
#include <QtCore/QObject>
#include <QtWidgets/QLayout>
#include <QtWidgets/QMenu>

#include <QtWidgets/QWidget>
#include <QtCore/QDebug>

class QContextMenuEvent;
class QToolBar;

namespace omnetpp {
namespace qtenv {

class InspectorFactory;


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
      InspectorType type;
      std::vector<cObject*> historyBack;
      std::vector<cObject*> historyForward;
      QAction *goBackAction = nullptr;
      QAction *goForwardAction = nullptr;
      QAction *goUpAction = nullptr;
      QAction *findObjects = nullptr;

      QMenu *inspectDropdownMenu;
      QMenu *copyDropdownMenu;

      static const QString PREF_GEOM;

      QString getFullPrefKey(const QString &pref, bool topLevel) const;
      QString getFullPrefKeyPerType(const QString &pref, bool topLevel) const;
      QVariant getPref(const QString &pref, const QVariant &defaultValue, bool perType = true) const;
      void setPref(const QString &pref, const QVariant &value, bool perType = true);

      virtual void refreshTitle();

      virtual void doSetObject(cObject *obj);
      virtual void doSetObjectSafe(cObject *obj);
      virtual void removeFromToHistory(cObject *obj);

      virtual void loadInitialGeometry();

      QSize sizeHint() const override;
      void closeEvent(QCloseEvent *) override;

      void addTopLevelToolBarActions(QToolBar *toolbar);
      void createInspectDropdownMenu();
      void createCopyDropdownMenu();

   public Q_SLOTS:
      virtual void setObject(cObject *object);
      virtual void goBack();
      virtual void goForward();
      virtual void inspectParent();
      virtual void findObjectsWithin();

   protected Q_SLOTS:
      // Context menu's action's slot
      virtual void goUpInto();

   Q_SIGNALS:
      void selectionChanged(cObject *object);
      void objectDoubleClicked(cObject *object);
      // Had to make the new object the first parameter, because the signal
      // can be bound to slots only taking a single parameter, and
      // this way those ones get only the new object, not the old one.
      void inspectedObjectChanged(cObject *newObject, cObject *oldObject);

   public:
      Inspector(QWidget *parent, bool isTopLevel, InspectorFactory *factory);
      virtual ~Inspector();
      virtual const char *getClassName() const;
      virtual bool supportsObject(cObject *object) const;

      virtual InspectorType getType() const {return type;}
      virtual bool isToplevelInspector() const {return isWindow();}

      virtual cObject *getObject() const {return object;}
      virtual bool canGoForward();
      virtual bool canGoBack();

      virtual void showWindow();

      virtual void refresh();
      virtual void postRefresh() {} // called on all inspectors as a "second stage", after refresh() was called on all of them

      virtual void objectDeleted(cObject *);
};

}  // namespace qtenv
}  // namespace omnetpp

#endif
