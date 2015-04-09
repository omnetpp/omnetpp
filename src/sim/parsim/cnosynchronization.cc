//=========================================================================
//  CNOSYNCHRONIZATION.CC - part of
//
//                  OMNeT++/OMNEST
//           Discrete System Simulation in C++
//
//  Author: Andras Varga, 2003
//          Dept. of Electrical and Computer Systems Engineering,
//          Monash University, Melbourne, Australia
//
//=========================================================================

/*--------------------------------------------------------------*
  Copyright (C) 2003-2008 Andras Varga
  Copyright (C) 2006-2008 OpenSim Ltd.

  This file is distributed WITHOUT ANY WARRANTY. See the file
  `license' for details on this and other legal matters.
*--------------------------------------------------------------*/


#include "omnetpp/cmessage.h"
#include "omnetpp/cmodule.h"
#include "omnetpp/cgate.h"
#include "omnetpp/cenvir.h"
#include "omnetpp/cparsimcomm.h"
#include "omnetpp/ccommbuffer.h"
#include "omnetpp/globals.h"
#include "omnetpp/regmacros.h"
#include "cnosynchronization.h"
#include "cparsimpartition.h"
#include "messagetags.h"

NAMESPACE_BEGIN

Register_Class(cNoSynchronization);


cNoSynchronization::cNoSynchronization() : cParsimProtocolBase()
{
}

cNoSynchronization::~cNoSynchronization()
{
}

void cNoSynchronization::startRun()
{
}

void cNoSynchronization::endRun()
{
}


cEvent *cNoSynchronization::takeNextEvent()
{
    // if no more local events, wait for something to come from other partitions
    if (sim->msgQueue.isEmpty())
    {
        EV << "no local events, waiting for something to arrive from other partitions\n";
        if (!receiveBlocking())
            return NULL;
    }
    else
    {
        receiveNonblocking();
    }

    cEvent *event = sim->msgQueue.removeFirst();
    return event;
}

void cNoSynchronization::putBackEvent(cEvent *event)
{
    sim->msgQueue.putBackFirst(event);
}

NAMESPACE_END

