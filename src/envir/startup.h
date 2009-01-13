//==========================================================================
//  STARTUP.H - part of
//                     OMNeT++/OMNEST
//            Discrete System Simulation in C++
//
//  Author: Andras Varga
//
//==========================================================================

/*--------------------------------------------------------------*
  Copyright (C) 1992-2008 Andras Varga
  Copyright (C) 2006-2008 OpenSim Ltd.

  This file is distributed WITHOUT ANY WARRANTY. See the file
  `license' for details on this and other legal matters.
*--------------------------------------------------------------*/

#ifndef __ENVIR_STARTUP_H
#define __ENVIR_STARTUP_H

#include "envirdefs.h"

class cConfiguration;

int setupUserInterface(int argc, char *argv[], cConfiguration *cfg);

#endif

