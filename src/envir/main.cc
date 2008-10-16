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
#include "bootenv.h"
#include "../utils/ver.h"

USING_NAMESPACE

BootEnv bootEnv;

//
// The main() function
//
ENVIR_API int main(int argc, char *argv[])
{
    cStaticFlag dummy;

    printf("OMNeT++/OMNEST Discrete Event Simulation  (C) 1992-2008 Andras Varga, OpenSim Ltd.\n");
    printf("Release: " OMNETPP_RELEASE ", build: " OMNETPP_BUILDID ", edition: " OMNETPP_EDITION "\n");
    printf("See the license for distribution terms and warranty disclaimer\n");

    evPtr = &bootEnv;
    int exitcode = bootEnv.run(argc, argv, NULL);
    printf("\nEnd.\n");

    return exitcode;
}

