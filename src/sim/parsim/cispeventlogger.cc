//=========================================================================
//  CISPEVENTLOGGER.CC - part of
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


#include <stdio.h>
#include "cispeventlogger.h"
#include "cidealsimulationprot.h"
#include "cparsimcomm.h"
#include "cplaceholdermod.h"
#include "cmodule.h"
#include "macros.h"

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
        throw new cRuntimeError("cISPEventLogger error: cannot open file `%s' for write", fname);
}

void cISPEventLogger::endRun()
{
    cNullMessageProtocol::endRun();
    fclose(fout);
}

void cISPEventLogger::processReceivedMessage(cMessage *msg, int destModuleId, int destGateId, int sourceProcId)
{
    msg->setPriority(sourceProcId);
    cParsimProtocolBase::processReceivedMessage(msg, destModuleId, destGateId, sourceProcId);
}

void cISPEventLogger::processOutgoingMessage(cMessage *msg, int procId, int moduleId, int gateId, void *data)
{
    if (msg->priority()!=0)
        throw new cRuntimeError("cISPEventLogger: outgoing message (%s)%s has nonzero priority() set -- "
                                "this conflicts with ISP which uses priority for its own purposes",
                                msg->className(), msg->name());
    cParsimProtocolBase::processOutgoingMessage(msg, procId, moduleId, gateId, data);
}

cMessage *cISPEventLogger::getNextEvent()
{
    cMessage *msg = cNullMessageProtocol::getNextEvent();

    if (msg->srcProcId()!=-1)  // received from another partition
    {
        // restore original priority
        msg->setPriority(0);

        // log event to file
        cIdealSimulationProtocol::ExternalEvent ev;
        ev.t = msg->arrivalTime();
        ev.srcProcId = msg->srcProcId();

        if (fwrite(&ev, sizeof(cIdealSimulationProtocol::ExternalEvent), 1, fout)<1)
            throw new cRuntimeError("cISPEventLogger error: file write failed (disk full?)");
    }

    return msg;
}


