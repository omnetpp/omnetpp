//=========================================================================
//
// CSCHEDULER.CC - part of
//                          OMNeT++
//           Discrete System Simulation in C++
//
//   Written by:  Andras Varga, 2003
//
//=========================================================================

/*--------------------------------------------------------------*
  Copyright (C) 2004 Andras Varga
  Monash University, Dept. of Electrical and Computer Systems Eng.
  Melbourne, Australia

  This file is distributed WITHOUT ANY WARRANTY. See the file
  `license' for details on this and other legal matters.
*--------------------------------------------------------------*/

#include "cscheduler.h"
#include "cmessage.h"
#include "csimul.h"
#include "cmsgheap.h"
#include "macros.h"


cScheduler::cScheduler()
{
    sim = NULL;
}

cScheduler::~cScheduler()
{
}

void cScheduler::setSimulation(cSimulation *_sim)
{
    sim = _sim;
}

//-----

Register_Class(cSequentialScheduler);

cMessage *cSequentialScheduler::getNextEvent()
{
    cMessage *msg = sim->msgQueue.peekFirst();
    if (!msg)
        throw new cTerminationException(eENDEDOK);
    return msg;
}


