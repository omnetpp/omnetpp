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
  Copyright (C) 1992-2015 Andras Varga
  Copyright (C) 2006-2015 OpenSim Ltd.

  This file is distributed WITHOUT ANY WARRANTY. See the file
  `license' for details on this and other legal matters.
*--------------------------------------------------------------*/

#include <cstdlib>
#include "envirdefs.h"

NAMESPACE_BEGIN
namespace envir {
extern "C" ENVIR_API int evMain(int argc, char *argv[]);
} // namespace envir
NAMESPACE_END

int main(int argc, char *argv[])
{
    // call the main function and start the simulation
    return OPP::envir::evMain(argc, argv);
}

