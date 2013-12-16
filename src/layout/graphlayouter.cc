//==========================================================================
//  GRAPHLAYOUTER.CC - part of
//                     OMNeT++/OMNEST
//            Discrete System Simulation in C++
//
//  Author: Levente Meszaros, Andras Varga
//
//==========================================================================

/*--------------------------------------------------------------*
  Copyright (C) 2006-2008 OpenSim Ltd.

  This file is distributed WITHOUT ANY WARRANTY. See the file
  `license' for details on this and other legal matters.
*--------------------------------------------------------------*/

#include <time.h>
#include "graphlayouter.h"

NAMESPACE_BEGIN

BasicGraphLayouterEnvironment::BasicGraphLayouterEnvironment()
{
    timeout = 0;
    startTime = time(NULL);
}

bool BasicGraphLayouterEnvironment::okToProceed()
{
    return timeout == 0 || time(NULL)-startTime <= timeout;
}

bool BasicGraphLayouterEnvironment::getBoolParameter(const char *name, int index, bool defaultValue)
{
    if (parameters.find(name) != parameters.end())
        return parameters[name] != 0;
    else
        return defaultValue;
}

long BasicGraphLayouterEnvironment::getLongParameter(const char *name, int index, long defaultValue)
{
    if (parameters.find(name) != parameters.end())
        return (long) parameters[name];
    else
        return defaultValue;
}

double BasicGraphLayouterEnvironment::getDoubleParameter(const char *name, int index, double defaultValue)
{
    if (parameters.find(name) != parameters.end())
        return parameters[name];
    else
        return defaultValue;
}

//-------------------------------

GraphLayouter::GraphLayouter()
{
    width = height = border = 0;
    environment = NULL;
}

void GraphLayouter::setSize(double w, double h, double bd)
{
#ifdef TRACE_LAYOUTER
    TRACE("GraphLayouter::setSize(w: %g, h: %g, bd: %g)", w, h, bd);
#endif
    width = w; height = h; border = bd;

    if ((width!=0 && width < 2*border) || (height!=0 && height < 2*border))
        throw opp_runtime_error("GraphLayouter::setSize(): required width or height smaller than 2*border");
}

NAMESPACE_END

