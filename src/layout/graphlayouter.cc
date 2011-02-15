//==========================================================================
//  GRAPHLAYOUTER.CC - part of
//                     OMNeT++/OMNEST
//            Discrete System Simulation in C++
//
//  Author: Levente Meszaros
//
//==========================================================================

/*--------------------------------------------------------------*
  Copyright (C) 2006-2008 OpenSim Ltd.

  This file is distributed WITHOUT ANY WARRANTY. See the file
  `license' for details on this and other legal matters.
*--------------------------------------------------------------*/

#include "graphlayouter.h"

USING_NAMESPACE


GraphLayouter::GraphLayouter()
{
    width = height = border = 0;
}

void GraphLayouter::setSize(double w, double h, double bd)
{
    width = w; height = h; border = bd;

    if ((width!=0 && width < 2*border) || (height!=0 && height < 2*border))
        throw opp_runtime_error("GraphLayouter::setSize(): required width or height smaller than 2*border");
}


