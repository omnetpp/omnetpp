//=========================================================================
//  CIDEALSIMULATIONPROT.H - part of
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

#ifndef __CIDEALSIMULATIONPROT_H__
#define __CIDEALSIMULATIONPROT_H__

#include "cparsimprotocolbase.h"

// forward declarations
class cCommBuffer;

/**
 * Implements the Ideal Simulation Protocol, described in the paper
 * "Performance Evaluation of Conservative Algoritms in Parallel
 * Simulation Languages" by Bagrodia et al.

 * This is not a real synchronization protocol, but rather a research
 * tool to measure efficiency of other synchonization protocols like
 * the null message algorithm.
 *
 * @ingroup Parsim
 */
class cIdealSimulationProtocol : public cParsimProtocolBase
{
  public:
    // stores one external event, as needed for the ISP algorithm
    struct ExternalEvent {
        simtime_t t;    // time of event
        int srcProcId;  // origin of event
    };

  protected:
    bool debug;
    FILE *fin;  // the event log file
    ExternalEvent nextExternalEvent; // holds the next event

    // preloaded table of event times
    ExternalEvent *table; // array to hold values
    int tableSize;        // allocated size of table
    int numItems;         // num in items in table (numItems<tableSize possible)
    int nextPos;          // position of 1st unused item in table

  protected:
    // process cMessages received from other partitions; see same method in2
    // cISPEventLogger for more explanation
    virtual void processReceivedMessage(cMessage *msg, int destModuleId, int destGateId, int sourceProcId);

    // read an event from event log file
    virtual void readNextRecordedEvent();

  public:
    /**
     * Constructor.
     */
    cIdealSimulationProtocol();

    /**
     * Destructor.
     */
    virtual ~cIdealSimulationProtocol();

    /**
     * Called at the beginning of a simulation run.
     */
    virtual void startRun();

    /**
     * Called at the end of a simulation run.
     */
    virtual void endRun();

    /**
     * Scheduler function.
     */
    virtual cMessage *getNextEvent();
};

#endif

