//==========================================================================
//  OPP_RUN.CC - part of
//
//                     OMNeT++/OMNEST
//             Discrete System Simulation in C++
//
//==========================================================================

/*--------------------------------------------------------------*
  Copyright (C) 1992-2005 Andras Varga

  This file is distributed WITHOUT ANY WARRANTY. See the file
  `license' for details on this and other legal matters.
*--------------------------------------------------------------*/

#include "cenvirimpl.h"

USING_NAMESPACE

static void nothing()
{
    // main() is part of the envir library, so here there's nothing to
    // do except creating a reference to the envir library
    cEnvirImpl dummy;
    (void)dummy;
}


