//==========================================================================
//  LAYOUTERENV.CC - part of
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
#include <cstdlib>
#include <cmath>
#include <cassert>

#include "omnetpp/cmodule.h"
#include "omnetpp/cdisplaystring.h"
#include "layouterenv.h"
#include "tklib.h"
#include "tkutil.h"

namespace omnetpp {
namespace tkenv {

TkenvGraphLayouterEnvironment::TkenvGraphLayouterEnvironment(Tcl_Interp *interp, cModule *parentModule, const cDisplayString& displayString)
    : displayString(displayString)
{
    this->interp = interp;
    this->parentModule = parentModule;
    widgetToGrab = nullptr;
    canvas = nullptr;
    interp = nullptr;

    beginTime = opp_get_monotonic_clock_usecs();
    lastCheck = opp_get_monotonic_clock_usecs();
    grabActive = false;
}

bool TkenvGraphLayouterEnvironment::getBoolParameter(const char *tagName, int index, bool defaultValue)
{
    return resolveBoolDispStrArg(displayString.getTagArg(tagName, index), parentModule, defaultValue);
}

long TkenvGraphLayouterEnvironment::getLongParameter(const char *tagName, int index, long defaultValue)
{
    return resolveLongDispStrArg(displayString.getTagArg(tagName, index), parentModule, defaultValue);
}

double TkenvGraphLayouterEnvironment::getDoubleParameter(const char *tagName, int index, double defaultValue)
{
    return resolveDoubleDispStrArg(displayString.getTagArg(tagName, index), parentModule, defaultValue);
}

void TkenvGraphLayouterEnvironment::clearGraphics()
{
    if (inspected()) {
        CHK(Tcl_VarEval(interp, canvas, " delete all", TCL_NULL));
    }
}

void TkenvGraphLayouterEnvironment::showGraphics(const char *text)
{
    if (inspected()) {
        CHK(Tcl_VarEval(interp, canvas, " raise node", TCL_NULL));
        CHK(Tcl_VarEval(interp, "layouter:debugDrawFinish ", canvas, " {", text, "}", TCL_NULL));
    }
}

void TkenvGraphLayouterEnvironment::drawText(double x, double y, const char *text, const char *tags, const char *color)
{
    if (inspected()) {
        char coords[100];
        sprintf(coords, "%d %d", (int)x, (int)y);
        CHK(Tcl_VarEval(interp, canvas, " create text ", coords, " -text ", text, " -fill ", color, " -tag ", tags, TCL_NULL));
    }
}

void TkenvGraphLayouterEnvironment::drawLine(double x1, double y1, double x2, double y2, const char *tags, const char *color)
{
    if (inspected()) {
        char coords[100];
        sprintf(coords, "%d %d %d %d", (int)x1, (int)y1, (int)x2, (int)y2);
        CHK(Tcl_VarEval(interp, canvas, " create line ", coords, " -fill ", color, " -tag ", tags, TCL_NULL));
    }
}

void TkenvGraphLayouterEnvironment::drawRectangle(double x1, double y1, double x2, double y2, const char *tags, const char *color)
{
    if (inspected()) {
        char coords[100];
        sprintf(coords, "%d %d %d %d", (int)x1, (int)y1, (int)x2, (int)y2);
        CHK(Tcl_VarEval(interp, canvas, " create rect ", coords, " -outline ", color, " -tag ", tags, TCL_NULL));
    }
}

bool TkenvGraphLayouterEnvironment::okToProceed()
{
    //
    // Strategy: do not interact with UI for up to 3 seconds. At the end of the
    // 3rd second, start grab on the "STOP" button, and read its state
    // occasionally (5 times per second). At the end (in cleanup()) we have to
    // release the grab. Do not set a grab in Express mode (i.e. if widgetToGrab==nullptr),
    // because Express mode's large STOP button already has one.
    //
    int64_t now = opp_get_monotonic_clock_usecs();

    if ((now - beginTime) < 3000 * 1000)
        return true;  // no UI interaction for up to 3 sec

    if (!grabActive && widgetToGrab) {
        // start grab
        grabActive = true;
        Tcl_SetVar(interp, "stoplayouting", "0", TCL_GLOBAL_ONLY);
        CHK(Tcl_VarEval(interp, "layouter:startGrab ", widgetToGrab, TCL_NULL));
    }

    // only check the UI once in a while (every 200 ms)
    if ((now - lastCheck) < 200 * 1000)
        return true;
    lastCheck = now;

    // process UI events; we assume that a "grab" is in effect to the Stop button
    // (i.e. the user can only interact with the Stop button, but nothing else)
    CHK(Tcl_VarEval(interp, "update\n", TCL_NULL));
    const char *var = Tcl_GetVar(interp, "stoplayouting", TCL_GLOBAL_ONLY);
    bool stopNow = var && var[0] && var[0] != '0';
    return !stopNow;
}

void TkenvGraphLayouterEnvironment::cleanup()
{
    if (inspected()) {
        CHK(Tcl_VarEval(interp,
                        canvas, " delete all\n",
                        canvas, " config -scrollregion {0 0 1 1}\n",
                        canvas, " xview moveto 0\n",
                        canvas, " yview moveto 0\n",
                        TCL_NULL));
    }
    if (grabActive) {
        CHK(Tcl_VarEval(interp, "layouter:releaseGrab ", widgetToGrab, TCL_NULL));
    }
}

}  // namespace tkenv
}  // namespace omnetpp

