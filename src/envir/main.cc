//==========================================================================
//  MAIN.CC - part of
//                     OMNeT++/OMNEST
//             Discrete System Simulation in C++
//
//  Function main()
//
//==========================================================================

/*--------------------------------------------------------------*
  Copyright (C) 1992-2005 Andras Varga

  This file is distributed WITHOUT ANY WARRANTY. See the file
  `license' for details on this and other legal matters.
*--------------------------------------------------------------*/

#include <stdio.h>
#include "cownedobject.h"
#include "cenvirimpl.h"
#include "../utils/ver.h"

USING_NAMESPACE


//
// The main() function
//
ENVIR_API int main(int argc, char *argv[])
{
    cStaticFlag dummy;

    printf("OMNeT++/OMNEST Discrete Event Simulation  (C) 1992-2005 Andras Varga\n");
    printf("Release: " OMNETPP_RELEASE ", edition: " OMNETPP_EDITION "\n");
    printf("See the license for distribution terms and warranty disclaimer\n");

    cEnvirImpl *env = new cEnvirImpl();
    evPtr = env;
    ev.setup(argc,argv);
    int ret = ev.run();
    ev.shutdown();
    //XXX delete env;
    //XXX evPtr = NULL;

    printf("\nEnd run of OMNeT++\n");
    return ret;
}

