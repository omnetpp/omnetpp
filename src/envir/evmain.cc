//==========================================================================
//  MAIN.CC - part of
//                     OMNeT++/OMNEST
//             Discrete System Simulation in C++
//
//  Function main()
//
//==========================================================================

/*--------------------------------------------------------------*
  Copyright (C) 1992-2017 Andras Varga
  Copyright (C) 2006-2017 OpenSim Ltd.

  This file is distributed WITHOUT ANY WARRANTY. See the file
  `license' for details on this and other legal matters.
*--------------------------------------------------------------*/

#include "envirdefs.h"
#include "startup.h"
#include "omnetpp/cownedobject.h"

namespace omnetpp {
namespace envir {

//
// The "main()" function of the simulation
//
extern "C" ENVIR_API int evMain(int argc, char *argv[])
{
    cStaticFlag dummy;
    int exitCode = setupUserInterface(argc, argv);
    return exitCode;
}

}  // namespace envir
}  // namespace omnetpp

