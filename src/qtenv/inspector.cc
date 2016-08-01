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
  Copyright (C) 1992-2015 Andras Varga
  Copyright (C) 2006-2015 OpenSim Ltd.

  This file is distributed WITHOUT ANY WARRANTY. See the file
  `license' for details on this and other legal matters.
*--------------------------------------------------------------*/

#include "mainwindow.h"
#include <cstring>
#include <cmath>
#include <cassert>
#include <algorithm>
#include <QMenu>
#include <QToolButton>
#include <QContextMenuEvent>
#include <QBoxLayout>
#include <QToolBar>
#include "common/stringutil.h"
#include "omnetpp/cobject.h"
#include "qtenv.h"
#include "qtutil.h"
#include "inspector.h"
#include "inspectorfactory.h"
#include "inspectorutil.h"

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
        CASE(INSP_MODULEOUTPUT);
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
    CASE(INSP_MODULEOUTPUT);
    CASE(INSP_OBJECTTREE);
    return -1;
#undef CASE
}

//----

Inspector::Inspector(QWidget *parent, bool isTopLevel, InspectorFactory *f)
    : QWidget(parent, isTopLevel ? Qt::Dialog : Qt::Widget)
{
    inspectContextMenu = new QMenu(this);
    copyContextMenu = new QMenu(this);
    factory = f;
    object = nullptr;
    type = f->getInspectorType();
    isToplevelWindow = isTopLevel;

    if (!isTopLevel) {
        auto layout = new QGridLayout(parent);
        parent->setLayout(layout);
        layout->setMargin(0);
        layout->addWidget(this, 0, 0, 1, 1);
    }
}

Inspector::~Inspector()
{
    if (isToplevelWindow)
        setPref(PREF_GEOM, geometry());
    delete inspectContextMenu;
    delete copyContextMenu;
}

const char *Inspector::getClassName() const
{
    return opp_typename(typeid(*this));
}

bool Inspector::supportsObject(cObject *object) const
{
    return factory->supportsObject(object);
}

void Inspector::createInspectContextMenu()
{
    if(!object)
        return;

    auto typeList = InspectorUtil::supportedInspTypes(object);

    inspectContextMenu->clear();
    for(auto type : typeList)
    {
        bool state = type == this->type;
        QString label = InspectorUtil::getInspectMenuLabel(type);
        QAction *action = inspectContextMenu->addAction(label, getQtenv(), SLOT(inspect()));
        action->setDisabled(state);
        action->setData(QVariant::fromValue(ActionDataPair(object, type)));
    }
}

void Inspector::createCopyContextMenu()
{
    if(!object)
        return;

    copyContextMenu->clear();
    QAction *action = copyContextMenu->addAction("Copy Pointer With Cast (for Debugger)", getQtenv(), SLOT(utilitiesSubMenu()));
    action->setData(QVariant::fromValue(ActionDataPair(object, InspectorUtil::COPY_PTRWITHCAST)));
    action = copyContextMenu->addAction("Copy Pointer Value (for Debugger)", getQtenv(), SLOT(utilitiesSubMenu()));
    action->setData(QVariant::fromValue(ActionDataPair(object, InspectorUtil::COPY_PTR)));
    copyContextMenu->addSeparator();
    action = copyContextMenu->addAction("Copy Full Path", getQtenv(), SLOT(utilitiesSubMenu()));
    action->setData(QVariant::fromValue(ActionDataPair(object, InspectorUtil::COPY_FULLPATH)));
    action = copyContextMenu->addAction("Copy Name", getQtenv(), SLOT(utilitiesSubMenu()));
    action->setData(QVariant::fromValue(ActionDataPair(object, InspectorUtil::COPY_FULLNAME)));
    action = copyContextMenu->addAction("Copy Class Name", getQtenv(), SLOT(utilitiesSubMenu()));
    action->setData(QVariant::fromValue(ActionDataPair(object, InspectorUtil::COPY_CLASSNAME)));
}

void Inspector::doSetObject(cObject *obj)
{
    if (obj != object) {
        if (obj && !supportsObject(obj))
            throw cRuntimeError("Inspector %s doesn't support objects of class %s", getClassName(), obj->getClassName());
        cObject *oldObject = object;
        object = obj;
        if (findObjects)
            findObjects->setData(QVariant::fromValue(object));
        // create context menus
        createInspectContextMenu();
        createCopyContextMenu();

        // note that doSetObject() is always followed by refresh(), see setObject()
        emit inspectedObjectChanged(object, oldObject);
    }
}

void Inspector::showWindow()
{
    if (isToplevelWindow) {
        show();
        raise();
    }
}

void Inspector::refresh()
{
    if (isToplevelWindow)
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

QString Inspector::getFullPrefKey(const QString& pref, bool topLevel)
{
    return "InspectorPreferences/type" + QString::number(type)
            + (topLevel ? "-toplevel" : "-embedded") + "-"
            + (object ? getObjectShortTypeName(object) : "NULL") + "-"
            + pref;
}

QVariant Inspector::getPref(const QString& pref, const QVariant& defaultValue)
{
    auto value = getQtenv()->getPref(getFullPrefKey(pref, isTopLevel()));
    // inheriting from the embedded inspector if not found for the toplevel
    if (isTopLevel() && !value.isValid())
        value = getQtenv()->getPref(getFullPrefKey(pref, false));
    return value.isValid() ? value : defaultValue;
}

void Inspector::setPref(const QString& pref, const QVariant& value)
{
    getQtenv()->setPref(getFullPrefKey(pref, isTopLevel()), value);
}

void Inspector::refreshTitle()
{
    // update window title (only if changed -- always updating the title produces
    // unnecessary redraws under some window managers
    char newTitle[256];
    const char *prefix = getQtenv()->getWindowTitlePrefix();
    if (!object) {
        sprintf(newTitle, "%s N/A", prefix);
    }
    else {
        std::string fullPath = object->getFullPath();
        if (fullPath.length() <= 60)
            sprintf(newTitle, "%s(%.40s) %s", prefix, getObjectShortTypeName(object), fullPath.c_str());
        else
            sprintf(newTitle, "%s(%.40s) ...%s", prefix, getObjectShortTypeName(object), fullPath.c_str()+fullPath.length()-55);
    }
    if (windowTitle != newTitle) {
        windowTitle = newTitle;
        setWindowTitle(windowTitle.c_str());
    }
}

void Inspector::objectDeleted(cObject *obj)
{
    if (obj == object) {
        doSetObject(nullptr);
        refresh();
    }
    removeFromToHistory(obj);
}

void Inspector::setObject(cObject *obj)
{
    if (obj != object) {
        if (object != nullptr) {
            historyBack.push_back(object);
            historyForward.clear();
        }
        doSetObject(obj);
        if (obj && isNew) {
            firstObjectSet(obj);
            showWindow();
            isNew = false;
        }
        refresh();
    }
}

template<typename T>
void removeFromVector(std::vector<T>& vec, T value)
{
    vec.erase(std::remove(vec.begin(), vec.end(), value), vec.end());
}

void Inspector::removeFromToHistory(cObject *obj)
{
    removeFromVector(historyBack, obj);
    removeFromVector(historyForward, obj);
}

void Inspector::firstObjectSet(cObject *obj)
{
    if (isToplevelWindow) {
        auto geom = getPref(PREF_GEOM, QRect()).toRect();

        adjustSize();
        if (!geom.isNull()) {
            setGeometry(geom);
        }
    }
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
        doSetObject(newObj);
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
        doSetObject(newObj);
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
    if (isToplevelWindow)
        setPref(PREF_GEOM, geometry());

    getQtenv()->deleteInspector(this);
    getQtenv()->storeInspectors(false);
}

void Inspector::addTopLevelToolBarActions(QToolBar *toolbar)
{
    // general
    goBackAction = toolbar->addAction(QIcon(":/tools/icons/tools/back.png"), "Back", this, SLOT(goBack()));
    goForwardAction = toolbar->addAction(QIcon(":/tools/icons/tools/forward.png"), "Forward", this, SLOT(goForward()));
    goUpAction = toolbar->addAction(QIcon(":/tools/icons/tools/parent.png"), "Go to parent module", this, SLOT(inspectParent()));
    toolbar->addSeparator();

    QAction *action = toolbar->addAction(QIcon(":/tools/icons/tools/inspectas.png"), "Inspect");
    QToolButton* toolButton = dynamic_cast<QToolButton *>(toolbar->widgetForAction(action));
    toolButton->setMenu(inspectContextMenu);
    toolButton->setPopupMode(QToolButton::InstantPopup);

    action = toolbar->addAction(QIcon(":/tools/icons/tools/copyptr.png"), "Copy name, type or pointer");
    toolButton = dynamic_cast<QToolButton *>(toolbar->widgetForAction(action));
    toolButton->setMenu(copyContextMenu);
    toolButton->setPopupMode(QToolButton::InstantPopup);

    toolbar->addAction(getQtenv()->getMainWindow()->getFindObjectsAction());
}

}  // namespace qtenv
}  // namespace omnetpp

