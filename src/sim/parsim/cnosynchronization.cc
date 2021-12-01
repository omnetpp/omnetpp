//=========================================================================
//  CNOSYNCHRONIZATION.CC - part of
//
//                     OMNeT++/OMNEST
//            Discrete System Simulation in C++
//
//=========================================================================

/*--------------------------------------------------------------*
  Copyright (C) 1992-2017 Andras Varga
  Copyright (C) 2006-2017 OpenSim Ltd.

  This file is distributed WITHOUT ANY WARRANTY. See the file
  `license' for details on this and other legal matters.
*--------------------------------------------------------------*/

#include "omnetpp/cmessage.h"
#include "omnetpp/cmodule.h"
#include "omnetpp/cgate.h"
#include "omnetpp/cenvir.h"
#include "omnetpp/cfutureeventset.h"
#include "omnetpp/cparsimcomm.h"
#include "omnetpp/ccommbuffer.h"
#include "omnetpp/globals.h"
#include "omnetpp/regmacros.h"
#include "cnosynchronization.h"
#include "cparsimpartition.h"
#include "messagetags.h"

namespace omnetpp {

Register_Class(cNoSynchronization);

void cNoSynchronization::startRun()
{
}

void cNoSynchronization::endRun()
{
}

cEvent *cNoSynchronization::takeNextEvent()
{
    // if no more local events, wait for something to come from other partitions
    if (sim->getFES()->isEmpty()) {
        EV << "no local events, waiting for something to arrive from other partitions\n";
        if (!receiveBlocking())
            return nullptr;
    }
    else {
        receiveNonblocking();
    }

    cEvent *event = sim->getFES()->removeFirst();
    return event;
}

void cNoSynchronization::putBackEvent(cEvent *event)
{
    sim->getFES()->putBackFirst(event);
}

}  // namespace omnetpp

