//==========================================================================
//  MAIN.CC - part of
//                     OMNeT++/OMNEST
//             Discrete System Simulation in C++
//
//  Function main()
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

#include <stdio.h>
#include "cownedobject.h"
#include "envirdefs.h"
#include "startup.h"
#include "../common/ver.h"

NAMESPACE_BEGIN
//
// The "main()" function of the simulation
//
extern "C" ENVIR_API int evMain(int argc, char *argv[])
{
    cStaticFlag dummy;

    printf(OMNETPP_PRODUCT " Discrete Event Simulation  (C) 1992-2014 Andras Varga, OpenSim Ltd.\n");
    printf("Version: " OMNETPP_VERSION_STR ", build: " OMNETPP_BUILDID ", edition: " OMNETPP_EDITION "\n");
    printf("See the license for distribution terms and warranty disclaimer\n");

    int exitcode = OPP::setupUserInterface(argc, argv);

    printf("\nEnd.\n");

    return exitcode;
}

NAMESPACE_END

