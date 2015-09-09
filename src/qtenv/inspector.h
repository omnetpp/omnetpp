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
 * Layout that manages two children: a client that completely fills the
 * parent; and a toolbar that flots over the client in the top-right corner.
 */
class FloatingLayout : public QLayout
{
public:
    FloatingLayout(): QLayout(), child(nullptr), floatingItem(nullptr) {}
    FloatingLayout(QWidget *parent): QLayout(parent), child(nullptr), floatingItem(nullptr) {}
    ~FloatingLayout() {
        delete child;
        delete floatingItem;
    }

    void addItem(QLayoutItem *item) {
        if (!child) child = item;
        else if (!floatingItem) floatingItem = item;
        else throw std::runtime_error("only two items are accepted");
    }

    int count() const {
        if (!child) return 0;
        else if (!floatingItem) return 1;
        else return 2;
    }

    QLayoutItem *itemAt(int i) const {
        if (i==0) return child;
        else if (i==1) return floatingItem;
        else return nullptr;
    }

    QLayoutItem *takeAt(int i) {
        QLayoutItem *result = nullptr;
        if (i==1) {result = floatingItem; floatingItem = nullptr;}
        else if (i==0) {result = child; child = floatingItem; floatingItem = nullptr;}
        else throw std::runtime_error("illegal index, must be 0 or 1");
        return result;
    }

    QSize sizeHint() const {
        if (child) return child->sizeHint();
        else return QSize(100,100);
    }

    QSize minimumSize() const {
        if (child) return child->minimumSize();
        else return QSize(0,0);
    }
    
    void setFloatMargins(const QMargins &margins) {
        floatMargins = margins;
        invalidate();
    }

protected:
    QLayoutItem *child;
    QLayoutItem *floatingItem;
    QMargins floatMargins;
};


class FloatingToolbarLayout : public FloatingLayout
{
public:
    FloatingToolbarLayout(): FloatingLayout() {}
    FloatingToolbarLayout(QWidget *parent): FloatingLayout(parent) {}

    void setGeometry(const QRect &rect) {
        if (child)
            child->setGeometry(rect); // margin?
        if (floatingItem) {
            QSize size = floatingItem->sizeHint();
            int x = std::max(floatMargins.left(), rect.width() - size.width() - floatMargins.right());
            int width = std::min(rect.width() - floatMargins.left() - floatMargins.right(), size.width());
            int height = std::min(rect.height() - floatMargins.top() - floatMargins.bottom(), size.height());
            floatingItem->setGeometry(QRect(x, floatMargins.top(), width, height));
        }
    }
};

/**
 * Base class for inspectors.
 */
class QTENV_API Inspector : public QWidget
{
   Q_OBJECT

   protected:
      InspectorFactory *factory; // meta-object that describes this inspector class
      //TCLKILL Tcl_Interp *interp;     // Tcl interpreterz
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

   protected:
      virtual void refreshTitle();

      virtual void doSetObject(cObject *obj);
      virtual void removeFromToHistory(cObject *obj);

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
      typedef QPair<OPP::cObject*, int> ActionDataPair;

      Inspector(QWidget *parent, bool isTopLevel, InspectorFactory *factory);
      virtual ~Inspector();
      virtual const char *getClassName() const;
      virtual bool supportsObject(cObject *object) const;

      static std::string makeWindowName();

      virtual int getType() const {return type;}
      virtual const char *getWindowName() const {return "";} //TODO obsolete, remove! returned inspector's Tk widget
      virtual bool isToplevel() const {return isToplevelWindow;}

      virtual cObject *getObject() const {return object;}
      virtual bool canGoForward();
      virtual bool canGoBack();

      virtual void showWindow();

      virtual void refresh();
      virtual void redraw() {refresh();}
      virtual void commit() {}

      virtual void clearObjectChangeFlags() {}

      //TCLKILL virtual int inspectorCommand(int, const char **);

      virtual void objectDeleted(cObject *);
};

} // namespace qtenv
} // namespace omnetpp

#endif


