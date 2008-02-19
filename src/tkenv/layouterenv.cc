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
  Copyright (C) 1992-2005 Andras Varga

  This file is distributed WITHOUT ANY WARRANTY. See the file
  `license' for details on this and other legal matters.
*--------------------------------------------------------------*/

#include <string.h>
#include <stdlib.h>
#include <math.h>
#include <assert.h>

#include "layouterenv.h"
#include "tklib.h"
#include "modinsp.h"
#include "cmodule.h"
#include "cdisplaystring.h"

USING_NAMESPACE



TGraphLayouterEnvironment::TGraphLayouterEnvironment(cModule *parentModule, const cDisplayString& displayString)
    : displayString(displayString)
{
    this->parentModule = parentModule;

    canvas = NULL;
    interp = NULL;
}

bool TGraphLayouterEnvironment::getBoolParameter(const char *tagName, int index, bool defaultValue)
{
    return resolveBoolDispStrArg(displayString.getTagArg(tagName, index), parentModule, defaultValue);
}

long TGraphLayouterEnvironment::getLongParameter(const char *tagName, int index, long defaultValue)
{
    return resolveLongDispStrArg(displayString.getTagArg(tagName, index), parentModule, defaultValue);
}

double TGraphLayouterEnvironment::getDoubleParameter(const char *tagName, int index, double defaultValue)
{
    return resolveDoubleDispStrArg(displayString.getTagArg(tagName, index), parentModule, defaultValue);
}

void TGraphLayouterEnvironment::clearGraphics()
{
    if (inspected())
    {
        Tcl_VarEval(interp, canvas, " delete all", NULL);
    }
}

void TGraphLayouterEnvironment::showGraphics(const char *text)
{
    if (inspected())
    {
        Tcl_VarEval(interp, canvas, " raise node", NULL);
        Tcl_VarEval(interp, "layouter_debugDraw_finish ", canvas, " {", text, "}", NULL);
    }
}

void TGraphLayouterEnvironment::drawText(double x, double y, const char *text, const char *tags, const char *color)
{
    if (inspected())
    {
        char coords[100];
        sprintf(coords,"%d %d", (int)x, (int)y);
        Tcl_VarEval(interp, canvas, " create text ", coords, " -text ", text, " -fill ", color, " -tag ", tags, NULL);
    }
}

void TGraphLayouterEnvironment::drawLine(double x1, double y1, double x2, double y2, const char *tags, const char *color)
{
    if (inspected())
    {
        char coords[100];
        sprintf(coords,"%d %d %d %d", (int)x1, (int)y1, (int)x2, (int)y2);
        Tcl_VarEval(interp, canvas, " create line ", coords, " -fill ", color, " -tag ", tags, NULL);
    }
}

void TGraphLayouterEnvironment::drawRectangle(double x1, double y1, double x2, double y2, const char *tags, const char *color)
{
    if (inspected())
    {
        char coords[100];
        sprintf(coords,"%d %d %d %d", (int)x1, (int)y1, (int)x2, (int)y2);
        Tcl_VarEval(interp, canvas, " create rect ", coords, " -outline ", color, " -tag ", tags, NULL);
    }
}

void TGraphLayouterEnvironment::cleanup()
{
    if (inspected())
    {
        Tcl_VarEval(interp, canvas, " delete all\n",
        //Tcl_VarEval(interp, canvas, " delete node edge force\n",
                            canvas, " config -scrollregion {0 0 1 1}\n",
                            canvas, " xview moveto 0\n",
                            canvas, " yview moveto 0\n",
                            NULL);
    }
}
