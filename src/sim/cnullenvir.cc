//=========================================================================
//  CNULLENVIR.CC - part of
//
//                  OMNeT++/OMNEST
//           Discrete System Simulation in C++
//
//=========================================================================

/*--------------------------------------------------------------*
  Copyright (C) 1992-2017 Andras Varga
  Copyright (C) 2006-2017 OpenSim Ltd.

  This file is distributed WITHOUT ANY WARRANTY. See the file
  `license' for details on this and other legal matters.
*--------------------------------------------------------------*/

#include <algorithm>
#include "omnetpp/cchannel.h"
#include "omnetpp/cnullenvir.h"

using namespace omnetpp;

cNullEnvir::cNullEnvir(int ac, char **av, cConfiguration *c) :
    argc(ac), argv(av), cfg(c), rng(new cMersenneTwister())
{
}

cNullEnvir::~cNullEnvir()
{
    while (!listeners.empty()) {
        listeners.back()->listenerRemoved();
        listeners.pop_back();
    }
    delete cfg;
    delete rng;
}

