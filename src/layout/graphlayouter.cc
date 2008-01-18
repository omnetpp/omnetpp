//==========================================================================
//  GRAPHLAYOUTER.CC - part of
//
//                     OMNeT++/OMNEST
//            Discrete System Simulation in C++
//
//==========================================================================

/*--------------------------------------------------------------*
  Copyright (C) 1992-2005 Andras Varga

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


