//=========================================================================
//  GEOMETRY.CC - part of
//                  OMNeT++/OMNEST
//           Discrete System Simulation in C++
//
//=========================================================================

/*--------------------------------------------------------------*
  Copyright (C) 1992-2006 Andras Varga

  This file is distributed WITHOUT ANY WARRANTY. See the file
  `license' for details on this and other legal matters.
*--------------------------------------------------------------*/

#include "geometry.h"

USING_NAMESPACE

static double zero = 0;

double NaN = zero / zero;

double POSITIVE_INFINITY = 1 / zero;
