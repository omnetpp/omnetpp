//=========================================================================
//  CISPEVENTLOGGER.H - part of
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

#ifndef __OMNETPP_CISPEVENTLOGGER_H
#define __OMNETPP_CISPEVENTLOGGER_H

#include "cnullmessageprot.h"

namespace omnetpp {


/**
 * @brief Implements phase one for the Ideal Simulation Protocol (ISP),
 * namely, creating the log of "external" events. That log will be
 * used as by the ISP synchromization mechanism (cIdealSimulationProtocol).
 *
 * This class simply executes the simulation under the Null Message
 * Algorithm, and additionally records all "external" events to a file.
 *
 * @ingroup Parsim
 */
class SIM_API cISPEventLogger : public cNullMessageProtocol
{
  protected:
    FILE *fout = nullptr;  // the event log file (stores ExternalEvent's)

  protected:
    // Overridden to set message priority to sourceProcId. We do the same in
    // cIdealSimulationProtocol. This ensures that messages with equal
    // timestamps will get processed in the same order in both protocols,
    // whatever the concrete timing conditions.
    void processReceivedMessage(cMessage *msg, const SendOptions& options, int destModuleId, int destGateId, int sourceProcId) override;

  public:
    /**
     * Constructor.
     */
    cISPEventLogger() {}

    /**
     * Destructor.
     */
    virtual ~cISPEventLogger() {}

    /**
     * Called at the beginning of a simulation run.
     * The addition to the base class is opening the file.
     */
    virtual void startRun() override;

    /**
     * Called at the end of a simulation run.
     * The addition to the base class is closing the file.
     */
    virtual void endRun() override;

    /**
     * Overridden to check that the model doesn't set message priority which
     * we need for our own purposes.
     */
    void processOutgoingMessage(cMessage *msg, const SendOptions& options, int procId, int moduleId, int gateId, void *data) override;

    /**
     * Scheduler function. The addition to the base class is
     * recording the event to the file.
     */
    virtual cEvent *takeNextEvent() override;

    /**
     * Undo takeNextEvent() -- it comes from the cScheduler interface.
     */
    virtual void putBackEvent(cEvent *event) override;
};

}  // namespace omnetpp


#endif

