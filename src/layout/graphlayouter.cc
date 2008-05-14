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
    sizingMode = Free;
}

void GraphLayouter::setConfineToArea(int w, int h, int bd)
{
    width = w; height = h; border = bd;
    sizingMode = Confine;
}

void GraphLayouter::setScaleToArea(int w, int h, int bd)
{
    width = w; height = h; border = bd;
    sizingMode = Scale;
}


