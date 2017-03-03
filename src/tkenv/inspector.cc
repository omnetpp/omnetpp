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

#include <cstring>
#include <cmath>
#include <cassert>
#include <algorithm>
#include "common/stringutil.h"
#include "omnetpp/cobject.h"
#include "tkenv.h"
#include "tklib.h"
#include "inspector.h"
#include "inspectorfactory.h"

using namespace omnetpp::common;

namespace omnetpp {
namespace tkenv {

const char *insptypeNameFromCode(int code)
{
#define CASE(x)  case x: return #x;
    switch (code) {
        CASE(INSP_DEFAULT);
        CASE(INSP_OBJECT);
        CASE(INSP_GRAPHICAL);
        CASE(INSP_MODULEOUTPUT);
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
    return -1;
#undef CASE
}

//----

Inspector::Inspector(InspectorFactory *f)
{
    factory = f;
    interp = getTkenv()->getInterp();
    object = nullptr;
    type = f->getInspectorType();
    isToplevelWindow = false;
    closeRequested = false;

    windowName[0] = '\0';  // no window exists
}

Inspector::~Inspector()
{
    if (isToplevelWindow && windowName[0]) {
        CHK(Tcl_VarEval(interp, "inspector:destroy ", windowName, TCL_NULL));
    }
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

void Inspector::createWindow(const char *window, const char *geometry)
{
    strcpy(windowName, window);
    isToplevelWindow = true;
}

void Inspector::useWindow(const char *window)
{
    strcpy(windowName, window);
    windowTitle = "";
    isToplevelWindow = false;
}

void Inspector::doSetObject(cObject *obj)
{
    ASSERT2(windowName[0], "createWindow()/useWindow() needs to be called before setObject()");

    if (obj != object) {
        if (obj && !supportsObject(obj))
            throw cRuntimeError("Inspector %s doesn't support objects of class %s", getClassName(), obj->getClassName());
        object = obj;
        CHK(Tcl_VarEval(interp, "inspector:onSetObject ", windowName, TCL_NULL));
        refresh();
    }
}

void Inspector::showWindow()
{
    if (isToplevelWindow)
        CHK(Tcl_VarEval(interp, "inspector:show ", windowName, TCL_NULL));
}

void Inspector::refresh()
{
    if (isToplevelWindow)
        refreshTitle();
    CHK(Tcl_VarEval(interp, "inspector:refresh ", windowName, TCL_NULL));
}

void Inspector::refreshTitle()
{
    // update window title (only if changed -- always updating the title produces
    // unnecessary redraws under some window managers
    char newTitle[256];
    const char *prefix = getTkenv()->getWindowTitlePrefix();
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
        CHK(Tcl_VarEval(interp, "wm title ", windowName, " {", windowTitle.c_str(), "}", TCL_NULL));
    }
}

void Inspector::objectDeleted(cObject *obj)
{
    if (obj == object)
        doSetObject(nullptr);
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
    }
}

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

void Inspector::setEntry(const char *entry, const char *val)
{
    if (!val)
        val = "";
    CHK(Tcl_VarEval(interp, windowName, entry, " delete 0 end;",
                    windowName, entry, " insert 0 {", val, "}", TCL_NULL));
}

void Inspector::setLabel(const char *label, const char *val)
{
    if (!val)
        val = "";
    CHK(Tcl_VarEval(interp, windowName, label, " config -text {", val, "}", TCL_NULL));
}

const char *Inspector::getEntry(const char *entry)
{
    CHK(Tcl_VarEval(interp, windowName, entry, " get", TCL_NULL));
    return Tcl_GetStringResult(interp);
}

}  // namespace tkenv
}  // namespace omnetpp

