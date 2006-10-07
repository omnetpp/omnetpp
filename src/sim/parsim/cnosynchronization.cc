//=========================================================================
//  CNOSYNCHRONIZATION.CC - part of
//
//                  OMNeT++/OMNEST
//           Discrete System Simulation in C++
//
//   Written by:  Andras Varga, 2003
//
//=========================================================================

/*--------------------------------------------------------------*
  Copyright (C) 2003-2005 Andras Varga
  Monash University, Dept. of Electrical and Computer Systems Eng.
  Melbourne, Australia

  This file is distributed WITHOUT ANY WARRANTY. See the file
  `license' for details on this and other legal matters.
*--------------------------------------------------------------*/


#include "cmessage.h"
#include "cmodule.h"
#include "cgate.h"
#include "cenvir.h"
#include "cnosynchronization.h"
#include "cparsimpartition.h"
#include "cparsimcomm.h"
#include "ccommbuffer.h"
#include "messagetags.h"
#include "macros.h"

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


cMessage *cNoSynchronization::getNextEvent()
{
    // if no more local events, wait for something to come from other partitions
    if (sim->msgQueue.empty())
    {
        ev.printf("no local events, waiting for something to arrive from other partitions\n");
        if (!receiveBlocking())
            return NULL;
    }
    else
    {
        receiveNonblocking();
    }
    cMessage *msg = sim->msgQueue.peekFirst();
    return msg;
}


