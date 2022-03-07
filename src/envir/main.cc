//==========================================================================
//  main.cc - part of
//
//                     OMNeT++/OMNEST
//             Discrete System Simulation in C++
//
//==========================================================================

/*--------------------------------------------------------------*
  Copyright (C) 1992-2017 Andras Varga
  Copyright (C) 2006-2017 OpenSim Ltd.

  This file is distributed WITHOUT ANY WARRANTY. See the file
  `license' for details on this and other legal matters.
*--------------------------------------------------------------*/

#include <cstdlib>
#include "envirdefs.h"

#if defined(__MINGW32__)
int _CRT_glob = 0;  // Turn off runtime file globbing support on MinGW. The shell already handles file globbing on the command line.
#endif

namespace omnetpp {
namespace envir {
extern "C" ENVIR_API int evMain(int argc, char *argv[]);
}  // namespace envir
}  // namespace omnetpp

int main(int argc, char *argv[])
{
    // call the main function and start the simulation
    return omnetpp::envir::evMain(argc, argv);
}

