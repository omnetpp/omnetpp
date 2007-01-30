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

#ifdef _MSC_VER
#pragma warning(disable:4786)
#endif

#include <string.h>
#include <stdlib.h>
#include <math.h>
#include <assert.h>

#include "layouterenv.h"
#include "tklib.h"
#include "modinsp.h"
#include "cmodule.h"
#include "cdisplaystring.h"



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
    Tcl_VarEval(interp, canvas, " delete all", NULL);
}

void TGraphLayouterEnvironment::showGraphics(const char *text)
{
    Tcl_VarEval(interp, canvas, " raise node", NULL);
    Tcl_VarEval(interp, "layouter_debugDraw_finish ", canvas, " {", text, "}", NULL);
}

void TGraphLayouterEnvironment::drawText(int x, int y, const char *text, const char *tags, const char *color)
{
    char coords[100];
    sprintf(coords,"%d %d", x, y);
    Tcl_VarEval(interp, canvas, " create text ", coords, " -text ", text, " -fill ", color, " -tag ", tags, NULL);
}

void TGraphLayouterEnvironment::drawLine(int x1, int y1, int x2, int y2, const char *tags, const char *color)
{
    char coords[100];
    sprintf(coords,"%d %d %d %d", x1, y1, x2, y2);
    Tcl_VarEval(interp, canvas, " create line ", coords, " -fill ", color, " -tag ", tags, NULL);
}

void TGraphLayouterEnvironment::drawRectangle(int x1, int y1, int x2, int y2, const char *tags, const char *color)
{
    char coords[100];
    sprintf(coords,"%d %d %d %d", x1, y1, x2, y2);
    Tcl_VarEval(interp, canvas, " create rect ", coords, " -outline ", color, " -tag ", tags, NULL);
}

void TGraphLayouterEnvironment::cleanup()
{
    if (inspected())
    {
        Tcl_VarEval(interp, canvas, " delete node\n",
                            canvas, " delete edge\n",
                            canvas, " delete force\n",
                            canvas, " config -scrollregion {0 0 1 1}\n",
                            canvas, " xview moveto 0\n",
                            canvas, " yview moveto 0\n",
                            NULL);
    }
}
