//=========================================================================
//
// CISPEVENTLOGGER.CC - part of
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


#include <stdio.h>
#include "cispeventlogger.h"
#include "cparsimcomm.h"
#include "cplaceholdermod.h"
#include "cmodule.h"
#include "macros.h"

Register_Class(cISPEventLogger);


// helper function
inline bool cISPEventLogger::isExternalMessage(cMessage *msg)
{
    cModule *srcmod = sim->module(msg->senderModuleId());
    return dynamic_cast<cPlaceHolderModule *>(srcmod) != NULL;
}

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
    fout = fopen(fname,"w");
    if (!fout)
        throw new cException("cISPEventLogger error: cannot open file `%s' for write", fname);
}

void cISPEventLogger::endRun()
{
    cNullMessageProtocol::endRun();
    fclose(fout);
}

cMessage *cISPEventLogger::getNextEvent()
{
    cMessage *msg = cNullMessageProtocol::getNextEvent();

    if (isExternalMessage(msg))
    {
        // log event to file
        ExternalEvent ev;
        ev.t = msg->arrivalTime();
        ev.srcProcId = 0; // FIXME

        if (fwrite(&ev, sizeof(ev), 1, fout)<1)
            throw new cException("cISPEventLogger error: file write failed (disk full?)");
    }

    return msg;
}


