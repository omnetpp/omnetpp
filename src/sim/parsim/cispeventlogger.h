//=========================================================================
//
// CISPEVENTLOGGER.H - part of
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

#ifndef __CISPEVENTLOGGER_H__
#define __CISPEVENTLOGGER_H__

#include "cnullmessageprot.h"


/**
 * Implements phase one for the Ideal Simulation Protocol (ISP),
 * namely, creating the log of "external" events. That log will be
 * used as by the ISP synchromization mechanism (cIdealSimulationProtocol).
 *
 * This class simply executes the simulation under the Null Message
 * Algorithm, and additionally records all "external" events to a file.
 *
 * @ingroup Parsim
 */
class cISPEventLogger : public cNullMessageProtocol
{
  protected:
    // stores one external event, as needed for the ISP algorithm
    struct ExternalEvent {
        simtime_t t;    // time of event
        int srcProcId;  // origin of event
    };
    FILE *fout;  // the event log file

  protected:
    // helper
    bool isExternalMessage(cMessage *msg);

  public:
    /**
     * Constructor.
     */
    cISPEventLogger();

    /**
     * Destructor.
     */
    virtual ~cISPEventLogger();

    /**
     * Called at the beginning of a simulation run.
     * The addition to the base class is opening the file.
     */
    virtual void startRun();

    /**
     * Called at the end of a simulation run.
     * The addition to the base class is closing the file.
     */
    virtual void endRun();

    /**
     * Scheduler function. The addition to the base class is
     * recording the event to the file.
     */
    virtual cMessage *getNextEvent();
};

#endif

