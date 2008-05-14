//==========================================================================
//  OPP_RUN.CC - part of
//
//                     OMNeT++/OMNEST
//             Discrete System Simulation in C++
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

#include "bootenv.h"

USING_NAMESPACE

static void nothing()
{
    // main() is part of the envir library, so here there's nothing to
    // do except creating a reference to the envir library
    BootEnv dummy;
    (void)dummy;
}


