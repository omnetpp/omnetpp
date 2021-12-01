//=========================================================================
//  CIDEALSIMULATIONPROT.H - part of
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

#ifndef __OMNETPP_CIDEALSIMULATIONPROT_H
#define __OMNETPP_CIDEALSIMULATIONPROT_H

#include "cparsimprotocolbase.h"

namespace omnetpp {

class cCommBuffer;

/**
 * @brief Implements the Ideal Simulation Protocol, described in the paper
 * "Performance Evaluation of Conservative Algoritms in Parallel
 * Simulation Languages" by Bagrodia et al.

 * This is not a real synchronization protocol, but rather a research
 * tool to measure efficiency of other synchonization protocols like
 * the null message algorithm.
 *
 * @ingroup Parsim
 */
class SIM_API cIdealSimulationProtocol : public cParsimProtocolBase
{
  public:
    // stores one external event, as needed for the ISP algorithm
    struct ExternalEvent {
        simtime_t t;    // time of event
        int srcProcId;  // origin of event
    };

  protected:
    bool debug;
    FILE *fin = nullptr;  // the event log file
    ExternalEvent nextExternalEvent; // holds the next event

    // preloaded table of event times
    ExternalEvent *table; // array to hold values
    int tableSize;        // allocated size of table
    int numItems = 0;     // num in items in table (numItems<tableSize possible)
    int nextPos = 0;      // position of 1st unused item in table

  protected:
    // process cMessages received from other partitions; see same method in2
    // cISPEventLogger for more explanation
    virtual void processReceivedMessage(cMessage *msg, const SendOptions& options, int destModuleId, int destGateId, int sourceProcId) override;

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
    virtual void startRun() override;

    /**
     * Called at the end of a simulation run.
     */
    virtual void endRun() override;

    /**
     * Scheduler function.
     */
    virtual cEvent *takeNextEvent() override;

    /**
     * Undo takeNextEvent() -- it comes from the cScheduler interface.
     */
    virtual void putBackEvent(cEvent *event) override;
};

}  // namespace omnetpp


#endif

