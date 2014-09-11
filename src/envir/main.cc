//==========================================================================
//  main.cc - part of
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

#include <stdlib.h>
#include "envirdefs.h"

NAMESPACE_BEGIN
extern "C" ENVIR_API int evMain(int argc, char *argv[]);
NAMESPACE_END

int main(int argc, char *argv[])
{
    // call the main function and start the simulation
    return OPP::evMain(argc, argv);
}

