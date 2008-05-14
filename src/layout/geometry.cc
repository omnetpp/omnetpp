//=========================================================================
//  GEOMETRY.CC - part of
//                  OMNeT++/OMNEST
//           Discrete System Simulation in C++
//
//  Author: Levente Meszaros
//
//=========================================================================

/*--------------------------------------------------------------*
  Copyright (C) 2006-2008 OpenSim Ltd.

  This file is distributed WITHOUT ANY WARRANTY. See the file
  `license' for details on this and other legal matters.
*--------------------------------------------------------------*/

#include "geometry.h"

USING_NAMESPACE

static double zero = 0;

double NaN = zero / zero;

double POSITIVE_INFINITY = 1 / zero;
