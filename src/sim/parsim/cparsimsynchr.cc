//=========================================================================
//  CPARSIMSYNCHR.CC - part of
//
//                  OMNeT++/OMNEST
//           Discrete System Simulation in C++
//
//=========================================================================

/*--------------------------------------------------------------*
  Copyright (C) 2003-2017 Andras Varga
  Copyright (C) 2006-2017 OpenSim Ltd.

  This file is distributed WITHOUT ANY WARRANTY. See the file
  `license' for details on this and other legal matters.
*--------------------------------------------------------------*/

#include "omnetpp/csimulation.h"
#include "omnetpp/cfutureeventset.h"
#include "cparsimsynchr.h"

using namespace omnetpp;

std::string cParsimSynchronizer::str() const
{
    return "parsim scheduler";
}

cEvent *cParsimSynchronizer::guessNextEvent()
{
    return sim->getFES()->peekFirst();
}

