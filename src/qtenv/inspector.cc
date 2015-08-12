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
#include <QContextMenuEvent>
#include "common/stringutil.h"
#include "omnetpp/cobject.h"
#include "qtenv.h"
#include "tklib.h"
#include "inspector.h"
#include "inspectorfactory.h"
#include "inspectorutil.h"
#include <QBoxLayout>

#define emit

using namespace OPP::common;

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

// ----

Inspector::Inspector(QWidget *parent, bool isTopLevel, InspectorFactory *f)
    : QWidget(parent, isTopLevel ? Qt::Tool : Qt::Widget)
{
    factory = f;
    //TCLKILL interp = getQtenv()->getInterp();
    object = nullptr;
    type = f->getInspectorType();
    isToplevelWindow = isTopLevel;

    if (isTopLevel) {
        show();
    } else {
        auto layout = new QGridLayout(parent);
        parent->setLayout(layout);
        layout->setMargin(0);
        layout->addWidget(this, 0, 0, 1, 1);
    }
}

Inspector::~Inspector()
{
}

const char *Inspector::getClassName() const
{
    return opp_typename(typeid(*this));
}

bool Inspector::supportsObject(cObject *object) const
{
    return factory->supportsObject(object);
}

std::string Inspector::makeWindowName()
{
    static int counter = 0;
    return opp_stringf(".inspector%d", counter++);
}

void Inspector::doSetObject(cObject *obj)
{
    if (obj != object) {
        if (obj && !supportsObject(obj))
            throw cRuntimeError("Inspector %s doesn't support objects of class %s", getClassName(), obj->getClassName());
        object = obj;
        // note that doSetObject() is always followed by refresh(), see setObject()
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
        emit objectPicked(newObj);
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
        emit objectPicked(newObj);
    }
}

void Inspector::inspectParent()
{
    cObject *parentPtr = dynamic_cast<cComponent *>(object) ? ((cComponent *)object)->getParentModule() : object->getOwner();
    if(parentPtr == nullptr)
        return;

    //inspect in current inspector if possible (and allowed), otherwise open a new one
    if(supportsObject(parentPtr)) { //TODO && $config(reuse-inspectors)
        doSetObject(parentPtr);
        emit objectPicked(parentPtr);
    }
    else
        getQtenv()->inspect(parentPtr);
}
/*TCLKILL
int Inspector::inspectorCommand(int argc, const char **argv)
{
    if (argc != 1) {
        Tcl_SetResult(interp, TCLCONST("wrong number of args"), TCL_STATIC);
        return TCL_ERROR;
    }

    if (strcmp(argv[0], "cangoback") == 0)
        Tcl_SetResult(interp, TCLCONST(canGoBack() ? "1" : "0"), TCL_STATIC);
    else if (strcmp(argv[0], "cangoforward") == 0)
        Tcl_SetResult(interp, TCLCONST(canGoForward() ? "1" : "0"), TCL_STATIC);
    else if (strcmp(argv[0], "goback") == 0)
        goBack();
    else if (strcmp(argv[0], "goforward") == 0)
        goForward();
    else {
        Tcl_SetResult(interp, TCLCONST("unknown inspector command"), TCL_STATIC);
        return TCL_ERROR;
    }
    return TCL_OK;
}
*/
void Inspector::goUpInto()
{
    QVariant variant = static_cast<QAction *>(QObject::sender())->data();
    if (variant.isValid()) {
        cObject *object = variant.value<cObject*>();
        setObject(object);
    }
}

void Inspector::inspectObject(cObject *object, int type)
{
    if (!object)
        return;

    getQtenv()->inspect(object, type, true);
}

void Inspector::openInspector()
{
    QVariant variant = static_cast<QAction *>(QObject::sender())->data();
    if (variant.isValid()) {
        QPair<cObject *, int> objTypePair = variant.value<QPair<cObject *, int> >();
        inspectObject(objTypePair.first, objTypePair.second);
    }
}

void Inspector::openInspector(cObject *object, int type)
{
    inspectObject(object, type);
}

void Inspector::runUntilModule()
{
    QVariant variant = static_cast<QAction *>(QObject::sender())->data();
    if (variant.isValid()) {
        QPair<cObject *, int> objTypePair = variant.value<QPair<cObject *, int> >();
        runSimulationLocal(objTypePair.second, objTypePair.first);
    }
}

void Inspector::runSimulationLocal(int runMode, cObject *object)
{
    MainWindow::eMode mode = MainWindow::runModeToMode(runMode);
    MainWindow *mainWindow = getQtenv()->getMainWindow();
    if (mainWindow->isRunning()) {
        mainWindow->setGuiForRunmode(mode);
        getQtenv()->setSimulationRunMode(runMode);
        mainWindow->setRunUntilModule(this);
    }
    else {
        if (!mainWindow->networkReady())
            return;
        mainWindow->setGuiForRunmode(mode, this);
        if (object == nullptr)
            object = this->getObject();

        cModule *mod = dynamic_cast<cModule *>(object);
        if (!mod) {
            // TODO log "object is not a module"
            return;
        }
        getQtenv()->runSimulation(runMode, 0, 0, nullptr, mod);
        mainWindow->setGuiForRunmode(MainWindow::NOT_RUNNING);
    }
}

void Inspector::runUntilMessage()
{
    QVariant variant = static_cast<QAction *>(QObject::sender())->data();
    if (variant.isValid()) {
        QPair<cObject *, int> objTypePair = variant.value<QPair<cObject *, int> >();
        getQtenv()->getMainWindow()->runUntilMsg(static_cast<cMessage *>(objTypePair.first), objTypePair.second);
    }
}

void Inspector::excludeMessage()
{
    QVariant variant = static_cast<QAction *>(QObject::sender())->data();
    if (variant.isValid())
        getQtenv()->getMainWindow()->excludeMessageFromAnimation(variant.value<cObject *>());
}

void Inspector::utilitiesSubMenu()
{
    QVariant variant = static_cast<QAction *>(QObject::sender())->data();
    if (variant.isValid()) {
        QPair<cObject *, int> objTypePair = variant.value<QPair<cObject *, int> >();
        InspectorUtil::copyToClipboard(static_cast<cMessage *>(objTypePair.first), objTypePair.second);
    }
}

void Inspector::closeEvent(QCloseEvent *) {
    getQtenv()->deleteInspector(this);
}

} // namespace qtenv
} // namespace omnetpp

