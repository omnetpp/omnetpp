//=========================================================================
//
// CPROXYGATE.CC - part of
//                          OMNeT++
//           Discrete System Simulation in C++
//
//   Written by:  Andras Varga, 2003
//
//=========================================================================

/*--------------------------------------------------------------*
  Copyright (C) 2003 Andras Varga
  Monash University, Dept. of Electrical and Computer Systems Eng.
  Melbourne, Australia

  This file is distributed WITHOUT ANY WARRANTY. See the file
  `license' for details on this and other legal matters.
*--------------------------------------------------------------*/

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "cproxygate.h"
#include "cparsimpartition.h"
#include "cmodule.h"
#include "cmessage.h"


cProxyGate::cProxyGate(const char *name, char type) : cGate(name, type)
{
    partition = NULL;
    remoteProcId = -1;
    remoteModuleId = -1;
    remoteGateId = -1;
    data = NULL;
}

cProxyGate::cProxyGate(const cProxyGate& gate) : cGate(NULL, 0)
{
    setName(gate.name());
    operator=(gate);
}

cProxyGate& cProxyGate::operator=(const cProxyGate& gate)
{
    if (this == &gate)
        return *this;

    cGate::operator = (gate);
    partition = gate.partition;
    remoteProcId = gate.remoteProcId;
    remoteModuleId = gate.remoteModuleId;
    remoteGateId = gate.remoteGateId;
    return *this;
}

void cProxyGate::info(char *buf)
{
    // info() string will be like:
    //    "(cProxyGate) outgate  remote:(procId=2,modId=7,gateId=1) <-- module.outgate  DE" (DER:DelayErrorDatarate)
    cObject::info( buf );

    char *b = buf;
    while(*b) b++;
    sprintf(b, "  remote:(procId=%d,modId=%d,gateId=%d)",
               remoteProcId, remoteModuleId, remoteGateId);
    while(*b) b++;

    // the following comes from cGate::info()
    char channel[5], *arrow, *s;
    cGate *g, *conng;

    if (typ=='O')
        {arrow = "--> "; g = togatep; conng = this;}
    else
        {arrow = "<-- "; g = fromgatep; conng = fromgatep;}

    s = channel;
    if (conng && conng->delay()) *s++='D';
    if (conng && conng->error()) *s++='E';
    if (conng && conng->datarate()) *s++='R';
    *s++ = ' ';
    *s = '\0';

    // append useful info to buf
    if (!g)
    {
        strcpy(b,"  "); b+=2;
        strcpy(b,arrow); b+=4;
        strcpy(b," (not connected)");
    }
    else
    {
        strcpy(b,"  "); b+=2;
        strcpy(b,arrow); b+=4;
        if (channel[0]!=' ')
           {strcpy(b,channel);strcat(b,arrow);while(*b) b++;}
        strcpy(b, g->ownerModule()==ownerModule()->parentModule() ?
                  "<parent>" : g->ownerModule()->fullName() );
        while(*b) b++; *b++ = '.';
        strcpy(b, g->fullName() );
    }
}

void cProxyGate::deliver(cMessage *msg, simtime_t t)
{
    ASSERT(togatep==NULL);
    ASSERT(partition!=NULL);
    if (remoteProcId==-1)
        throw new cException(this, "cannot deliver message '%s': not connected to remote gate", msg->name());

    msg->setArrivalTime(t);  // merge arrival time into message
    partition->processOutgoingMessage(msg, remoteProcId, remoteModuleId, remoteGateId, data);
    // delete msg; FIXME it must be deleted somewhere!!!
}

void cProxyGate::setRemoteGate(int procId, int moduleId, int gateId)
{
    remoteProcId = procId;
    remoteModuleId = moduleId;
    remoteGateId = gateId;
}


