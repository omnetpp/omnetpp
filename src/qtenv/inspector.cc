//==========================================================================
//  INSPECTOR.CC - part of
//
//                     OMNeT++/OMNEST
//            Discrete System Simulation in C++
//
//  Implementation of
//    inspectors
//
//==========================================================================

/*--------------------------------------------------------------*
  Copyright (C) 1992-2017 Andras Varga
  Copyright (C) 2006-2017 OpenSim Ltd.

  This file is distributed WITHOUT ANY WARRANTY. See the file
  `license' for details on this and other legal matters.
*--------------------------------------------------------------*/

#include "mainwindow.h"
#include <cstring>
#include <cmath>
#include <cassert>
#include <algorithm>
#include <QtWidgets/QMenu>
#include <QtWidgets/QToolButton>
#include <QtGui/QContextMenuEvent>
#include <QtWidgets/QBoxLayout>
#include <QtWidgets/QToolBar>
#include "common/stringutil.h"
#include "common/stlutil.h"
#include "omnetpp/cobject.h"
#include "qtenv.h"
#include "qtenvdefs.h"
#include "qtutil.h"
#include "inspector.h"
#include "inspectorfactory.h"
#include "inspectorutil.h"
#include "genericobjectinspector.h"

#define emit

using namespace omnetpp::common;

namespace omnetpp {
namespace qtenv {

//TODO these two functions are likely not needed any more
const char *insptypeNameFromCode(int code)
{
#define CASE(x)  case x: return #x;
    switch (code) {
        CASE(INSP_DEFAULT);
        CASE(INSP_OBJECT);
        CASE(INSP_GRAPHICAL);
        CASE(INSP_LOG);
        CASE(INSP_OBJECTTREE);
        default: return "?";
    }
#undef CASE
}

int insptypeCodeFromName(const char *name)
{
#define CASE(x)  if (strcmp(name, #x)==0) return x;
    CASE(INSP_DEFAULT);
    CASE(INSP_OBJECT);
    CASE(INSP_GRAPHICAL);
    CASE(INSP_LOG);
    CASE(INSP_OBJECTTREE);
    return -1;
#undef CASE
}

//----

// About the WindowType of toplevel inspectors:
//
// For quite a while, we used Qt::Dialog. This had a few problems:
//   - With certain window managers, all buttons on the title bar were missing.
//     (see https://dev.omnetpp.org/bugs/view.php?id=953 )
//   - On macOS, they weren't always kept on top of the MainWindow.
//
// Adding Qt::WindowStaysOnTopHint to a Qt::Dialog isn't a solution either,
// because on macOS, they will stay on top of _all_ the windows at all times,
// not just the MainWindow.
//
// With Qt::Tool, they have thinner border, and disappear on Mac when the
// MainWindow loses focus (the Qt::WA_MacAlwaysShowToolWindow flag does
// the same as above, not what we want), but this is the closest we ould get...
//
// We have also tried a few others (Drawer, Popup, etc...), but none were quite right.
//
// Tkenv did something else, it put Inspector windows on a so-called
// Utility Level, so they work best (and move with the MainWindow on Mac),
// but we couldn't find a way to do that without ugly ObjC code.
//
// This is a well-known and seemingly unsolved issue, see
// http://stackoverflow.com/questions/32216498 as well.
//
Inspector::Inspector(QWidget *parent, bool isTopLevel, InspectorFactory *f)
    : QWidget(parent, isTopLevel ? Qt::Tool : Qt::Widget)
{
    inspectDropdownMenu = new QMenu(this);
    copyDropdownMenu = new QMenu(this);
    factory = f;
    object = nullptr;
    type = f->getInspectorType();

    if (isTopLevel) {
        setAttribute(Qt::WA_DeleteOnClose);

        QAction *closeAction = new QAction("Close Inspector", this);
        closeAction->setShortcut(Qt::CTRL + Qt::Key_W);
        connect(closeAction, &QAction::triggered, this, &QWidget::close);
        addAction(closeAction);
    }
    else {
        auto layout = new QGridLayout(parent);
        parent->setLayout(layout);
        layout->setMargin(0);
        layout->addWidget(this, 0, 0, 1, 1);
    }
}

Inspector::~Inspector()
{
    if (isToplevelInspector())
        setPref(PREF_GEOM, geometry());
    delete inspectDropdownMenu;
    delete copyDropdownMenu;

    getQtenv()->inspectorDeleted(this);
}

const char *Inspector::getClassName() const
{
    return common::opp_typename(typeid(*this));
}

bool Inspector::supportsObject(cObject *object) const
{
    return factory->supportsObject(object);
}

void Inspector::createInspectDropdownMenu()
{
    if (!object)
        return;

    auto typeList = InspectorUtil::supportedInspTypes(object);

    inspectDropdownMenu->clear();

    inspectDropdownMenu->addAction("View in Embedded Object Inspector",
                                   getQtenv()->getMainObjectInspector(), SLOT(goUpInto()))
        ->setData(QVariant::fromValue(object));
    inspectDropdownMenu->addSeparator();

    for(auto type : typeList) {
        bool state = type == this->type;
        QString label = InspectorUtil::getInspectMenuLabel(type);
        QAction *action = inspectDropdownMenu->addAction(label, getQtenv(), SLOT(inspect()));
        action->setDisabled(state);
        action->setData(QVariant::fromValue(InspectActionData{object, type}));
    }
}

void Inspector::createCopyDropdownMenu()
{
    if (!object)
        return;

    copyDropdownMenu->clear();
    QAction *action = copyDropdownMenu->addAction("Copy Pointer With Cast (for Debugger)", getQtenv(), SLOT(utilitiesSubMenu()));
    action->setData(QVariant::fromValue(CopyActionData{object, COPY_PTRWITHCAST}));
    action = copyDropdownMenu->addAction("Copy Pointer Value (for Debugger)", getQtenv(), SLOT(utilitiesSubMenu()));
    action->setData(QVariant::fromValue(CopyActionData{object, COPY_PTR}));
    copyDropdownMenu->addSeparator();

    action = copyDropdownMenu->addAction("Copy Full Path", getQtenv(), SLOT(utilitiesSubMenu()));
    action->setData(QVariant::fromValue(CopyActionData{object, COPY_FULLPATH}));
    action = copyDropdownMenu->addAction("Copy Name", getQtenv(), SLOT(utilitiesSubMenu()));
    action->setData(QVariant::fromValue(CopyActionData{object, COPY_FULLNAME}));
    action = copyDropdownMenu->addAction("Copy Class Name", getQtenv(), SLOT(utilitiesSubMenu()));
    action->setData(QVariant::fromValue(CopyActionData{object, COPY_CLASSNAME}));
}

void Inspector::doSetObject(cObject *obj)
{
    if (obj && !supportsObject(obj))
        throw cRuntimeError("Inspector %s doesn't support objects of class %s", getClassName(), obj->getClassName());
    cObject *oldObject = object;
    object = obj;
    if (findObjects)
        findObjects->setData(QVariant::fromValue(object));
    // create context menus
    createInspectDropdownMenu();
    createCopyDropdownMenu();

    // note that doSetObject() is always followed by refresh(), see setObject()
    emit inspectedObjectChanged(object, oldObject);
}

void Inspector::doSetObjectSafe(cObject *obj)
{
    try {
        doSetObject(obj);
    }
    catch (std::exception &e) {
        getQtenv()->showException(e);
    }
}

void Inspector::showWindow()
{
    ASSERT(isToplevelInspector());
    show();
    raise();
    activateWindow();
}

void Inspector::refresh()
{
    if (isToplevelInspector())
        refreshTitle();

    if (goBackAction)
        goBackAction->setEnabled(canGoBack());
    if (goForwardAction)
        goForwardAction->setEnabled(canGoForward());
    if (goUpAction) {
        if (object) {
            cObject *parent = dynamic_cast<cComponent *>(object) ? ((cComponent *)object)->getParentModule() : object->getOwner();
            goUpAction->setEnabled(parent);
        }
        else {
            goUpAction->setEnabled(false);
        }
    }
}

const QString Inspector::PREF_GEOM = "geom";

QString Inspector::getFullPrefKey(const QString& pref, bool topLevel) const
{
    return "InspectorPreferences/type" + QString::number(type) + "-"
            + (topLevel ? "-toplevel" : "-embedded") + "-"
            + pref;
}

QString Inspector::getFullPrefKeyPerType(const QString& pref, bool topLevel) const
{
    return "InspectorPreferences/type" + QString::number(type)
            + (topLevel ? "-toplevel" : "-embedded") + "-"
            + (object ? getObjectShortTypeName(object, STRIPNAMESPACE_NONE) : "nullptr") + "-"
            + pref;
}

QVariant Inspector::getPref(const QString& pref, const QVariant& defaultValue, bool perType) const
{
    QString fullPrefKey = perType
                ? getFullPrefKeyPerType(pref, isTopLevel())
                : getFullPrefKey(pref, isTopLevel());
    QString embeddedFullPrefKey = perType
                ? getFullPrefKeyPerType(pref, false)
                : getFullPrefKey(pref, false);

    auto value = getQtenv()->getPref(fullPrefKey);
    // inheriting from the embedded inspector if not found for the toplevel
    if (isTopLevel() && !value.isValid())
        value = getQtenv()->getPref(embeddedFullPrefKey);
    return value.isValid() ? value : defaultValue;
}

void Inspector::setPref(const QString& pref, const QVariant& value, bool perType)
{
    QString key = perType ? getFullPrefKeyPerType(pref, isTopLevel()) : getFullPrefKey(pref, isTopLevel());
    getQtenv()->setPref(key, value);
}

void Inspector::refreshTitle()
{
    QString title = getQtenv()->getWindowTitlePrefix();
    if (!object)
        title += " N/A";
    else {
        std::string fullPath = object->getFullPath();

        if (fullPath.length() <= 60)
            title += QString("(%1) %2").arg(getObjectShortTypeName(object).left(40)).arg(fullPath.c_str());
            //sprintf(newTitle, "%s(%.40s) %s", prefix, , fullPath.c_str());
        else
            title += QString("(%1) ...%2").arg(getObjectShortTypeName(object).left(40)).arg(fullPath.c_str()+fullPath.length()-55);
            //sprintf(newTitle, "%s(%.40s) ...%s", prefix, getObjectShortTypeName(object), fullPath.c_str()+fullPath.length()-55);
    }

    setWindowTitle(title);
}

void Inspector::objectDeleted(cObject *obj)
{
    if (obj == object) {
        doSetObjectSafe(nullptr);
        refresh();
    }
    removeFromToHistory(obj);
}

void Inspector::setObject(cObject *obj)
{
    if (isNew) {
        doSetObjectSafe(obj);
        if (isToplevelInspector()) {
            loadInitialGeometry();
            showWindow();
        }
        refresh();
        isNew = false;
    }
    else if (obj != object) {
        if (object != nullptr) {
            historyBack.push_back(object);
            historyForward.clear();
        }
        doSetObjectSafe(obj);
        refresh();
    }
}

void Inspector::removeFromToHistory(cObject *obj)
{
    remove(historyBack, obj);
    remove(historyForward, obj);
}

void Inspector::loadInitialGeometry()
{
    ASSERT(isToplevelInspector());

    adjustSize();

    auto geom = getPref(PREF_GEOM, QRect()).toRect();
    if (!geom.isNull())
        setGeometry(geom);
}

QSize Inspector::sizeHint() const
{
    return QSize(400, 300);
}

bool Inspector::canGoForward()
{
    return !historyForward.empty();
}

bool Inspector::canGoBack()
{
    return !historyBack.empty();
}

void Inspector::goForward()
{
    if (!historyForward.empty()) {
        cObject *newObj = historyForward.back();
        historyForward.pop_back();
        if (object != nullptr)
            historyBack.push_back(object);
        doSetObjectSafe(newObj);
        refresh();
    }
}

void Inspector::goBack()
{
    if (!historyBack.empty()) {
        cObject *newObj = historyBack.back();
        historyBack.pop_back();
        if (object != nullptr)
            historyForward.push_back(object);
        doSetObjectSafe(newObj);
        refresh();
    }
}

void Inspector::inspectParent()
{
    cObject *parentPtr = dynamic_cast<cComponent *>(object) ? ((cComponent *)object)->getParentModule() : object->getOwner();
    if (parentPtr == nullptr)
        return;

    // inspect in current inspector if possible (and allowed), otherwise open a new one
    if (supportsObject(parentPtr)) {  // TODO && $config(reuse-inspectors)
        setObject(parentPtr);
    }
    else
        getQtenv()->inspect(parentPtr);
}

void Inspector::findObjectsWithin()
{
    emit getQtenv()->getMainWindow()->showFindObjectsDialog(object);
}

void Inspector::goUpInto()  // XXX weird name
{
    QVariant variant = static_cast<QAction *>(QObject::sender())->data();
    if (variant.isValid()) {
        cObject *object = variant.value<cObject *>();
        setObject(object);
    }
}

void Inspector::closeEvent(QCloseEvent *)
{
    ASSERT(isToplevelInspector());

    setPref(PREF_GEOM, geometry());

    // We have to call this prematurely, so the stored inspector list won't contain it.
    // The dtor will also call it, sure, but it is idempotent.
    getQtenv()->inspectorDeleted(this);
    getQtenv()->storeInspectors(false);
}

void Inspector::addTopLevelToolBarActions(QToolBar *toolbar)
{
    // general
    goBackAction = toolbar->addAction(QIcon(":/tools/back"), "Back", this, SLOT(goBack()));
    goForwardAction = toolbar->addAction(QIcon(":/tools/forward"), "Forward", this, SLOT(goForward()));
    goUpAction = toolbar->addAction(QIcon(":/tools/parent"), "Go to parent module", this, SLOT(inspectParent()));
    toolbar->addSeparator();

    QAction *action = toolbar->addAction(QIcon(":/tools/inspectas"), "Inspect");
    QToolButton* toolButton = dynamic_cast<QToolButton *>(toolbar->widgetForAction(action));
    toolButton->setMenu(inspectDropdownMenu);
    toolButton->setPopupMode(QToolButton::InstantPopup);

    action = toolbar->addAction(QIcon(":/tools/copyptr"), "Copy name, type or pointer");
    toolButton = dynamic_cast<QToolButton *>(toolbar->widgetForAction(action));
    toolButton->setMenu(copyDropdownMenu);
    toolButton->setPopupMode(QToolButton::InstantPopup);

    toolbar->addAction(QIcon(":/tools/findobj"), "Find/inspect objects", this, SLOT(findObjectsWithin()))
            ->setShortcut(Qt::CTRL | Qt::Key_S);
}

}  // namespace qtenv
}  // namespace omnetpp

