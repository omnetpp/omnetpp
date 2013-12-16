//=========================================================================
//  CISPEVENTLOGGER.CC - part of
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


#include <stdio.h>
#include "cispeventlogger.h"
#include "cidealsimulationprot.h"
#include "cparsimcomm.h"
#include "cplaceholdermod.h"
#include "cmodule.h"
#include "globals.h"
#include "regmacros.h"

NAMESPACE_BEGIN

Register_Class(cISPEventLogger);


cISPEventLogger::cISPEventLogger() : cNullMessageProtocol()
{
    fout = NULL;
}

cISPEventLogger::~cISPEventLogger()
{
}

void cISPEventLogger::startRun()
{
    cNullMessageProtocol::startRun();

    char fname[200];
    sprintf(fname, "ispeventlog-%d.dat", comm->getProcId());
    fout = fopen(fname,"wb");
    if (!fout)
        throw cRuntimeError("cISPEventLogger error: cannot open file `%s' for write", fname);
}

void cISPEventLogger::endRun()
{
    cNullMessageProtocol::endRun();
    fclose(fout);
}

void cISPEventLogger::processReceivedMessage(cMessage *msg, int destModuleId, int destGateId, int sourceProcId)
{
    msg->setSchedulingPriority(sourceProcId);
    cParsimProtocolBase::processReceivedMessage(msg, destModuleId, destGateId, sourceProcId);
}

void cISPEventLogger::processOutgoingMessage(cMessage *msg, int procId, int moduleId, int gateId, void *data)
{
    if (msg->getSchedulingPriority()!=0)
        throw cRuntimeError("cISPEventLogger: outgoing message (%s)%s has nonzero priority set -- "
                            "this conflicts with ISP which uses priority for its own purposes",
                            msg->getClassName(), msg->getName());
    cParsimProtocolBase::processOutgoingMessage(msg, procId, moduleId, gateId, data);
}

cMessage *cISPEventLogger::getNextEvent()
{
    cMessage *msg = cNullMessageProtocol::getNextEvent();

    if (msg->getSrcProcId()!=-1)  // received from another partition
    {
        // restore original priority
        msg->setSchedulingPriority(0);

        // log event to file
        cIdealSimulationProtocol::ExternalEvent e;
        e.t = msg->getArrivalTime();
        e.srcProcId = msg->getSrcProcId();

        if (fwrite(&e, sizeof(cIdealSimulationProtocol::ExternalEvent), 1, fout)<1)
            throw cRuntimeError("cISPEventLogger error: file write failed (disk full?)");
    }

    return msg;
}

NAMESPACE_END

