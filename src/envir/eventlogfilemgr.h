//==========================================================================
//  EVENTLOGFILEMGR.H - part of
//                     OMNeT++/OMNEST
//            Discrete System Simulation in C++
//
//  Author: Andras Varga, Levente Meszaros
//
//==========================================================================

/*--------------------------------------------------------------*
  Copyright (C) 1992-2017 Andras Varga
  Copyright (C) 2006-2017 OpenSim Ltd.

  This file is distributed WITHOUT ANY WARRANTY. See the file
  `license' for details on this and other legal matters.
*--------------------------------------------------------------*/

#ifndef __OMNETPP_ENVIR_EVENTLOGFILEMGR_H
#define __OMNETPP_ENVIR_EVENTLOGFILEMGR_H

#include "omnetpp/simkerneldefs.h"
#include "omnetpp/opp_string.h"
#include "omnetpp/envirext.h"
#include "envirdefs.h"
#include "objectprinter.h"
#include "intervals.h"

namespace omnetpp {

class cComponent;
class cModule;
class cChannel;

namespace envir {

/**
 * Responsible for writing the eventlog file.
 */
class ENVIR_API EventlogFileManager : public cIEventlogManager, public cISimulationLifecycleListener
{
  private:
    cEnvir *envir;
    std::string filename;
    FILE *feventlog;
    ObjectPrinter *objectPrinter;
    Intervals *recordingIntervals;
    eventnumber_t eventNumber;
    int entryIndex;
    int keyframeBlockSize;
    file_offset_t previousKeyframeFileOffset;
    bool isUserRecordingEnabled;
    bool isCombinedRecordingEnabled;  // combines several other enablement flags

    // keyframe data structures
    struct EventLogEntryReference
    {
        eventnumber_t eventNumber;
        int entryIndex;

        EventLogEntryReference()
        {
            this->eventNumber = -1;
            this->entryIndex = -1;
        }

        EventLogEntryReference(eventnumber_t eventNumber, int entryIndex)
        {
            this->eventNumber = eventNumber;
            this->entryIndex = entryIndex;
        }
    };

    struct EventLogEntryRange
    {
        eventnumber_t eventNumber;
        int beginEntryIndex;
        int endEntryIndex;

        EventLogEntryRange(eventnumber_t eventNumber, int beginEntryIndex, int endEntryIndex)
        {
            this->eventNumber = eventNumber;
            this->beginEntryIndex = beginEntryIndex;
            this->endEntryIndex = endEntryIndex;
        }

        void print(FILE *file)
        {
            if (beginEntryIndex == endEntryIndex)
                fprintf(file, "%" PRId64 ":%d", eventNumber, beginEntryIndex);
            else
                fprintf(file, "%" PRId64 ":%d-%d", eventNumber, beginEntryIndex, endEntryIndex);
        }
    };

    // keyframe state
    std::vector<eventnumber_t> consequenceLookaheadLimits;
    std::map<eventnumber_t, std::vector<EventLogEntryRange> > eventNumberToSimulationStateEventLogEntryRanges;
    std::map<cModule *, EventLogEntryReference> moduleToModuleDisplayStringChangedEntryReferenceMap;
    std::map<cChannel *, EventLogEntryReference> channelToConnectionDisplayStringChangedEntryReferenceMap;
    std::map<cMessage *, EventLogEntryReference> messageToBeginSendEntryReferenceMap;

  protected:
    /**
     * A cISimulationLifecycleListener method.
     */
    virtual void lifecycleEvent(SimulationLifecycleEventType eventType, cObject *details) override;

  public:
    explicit EventlogFileManager();
    virtual ~EventlogFileManager();

    virtual void configure();
    virtual bool isOpen() { return feventlog != nullptr; }
    virtual void open();
    virtual void close();
    virtual void remove();

    virtual void startRecording() override;
    virtual void stopRecording() override;
    virtual void flush() override;

    virtual bool hasRecordingIntervals();
    virtual void clearRecordingIntervals();

    virtual void recordSimulation();
    virtual void recordInitialize();
    virtual void recordMessages();
    virtual void recordModules(cModule *module);
    virtual void recordConnections(cModule *module);

    virtual const char *getFileName() const override { return filename.c_str(); }

    /** @name Functions called from cEnvir's similar functions */
    //@{
    virtual void simulationEvent(cEvent *event) override;
    virtual void bubble(cComponent *component, const char *text) override;
    virtual void messageScheduled(cMessage *msg) override;
    virtual void messageCancelled(cMessage *msg) override;
    virtual void beginSend(cMessage *msg) override;
    virtual void messageSendDirect(cMessage *msg, cGate *toGate, simtime_t propagationDelay, simtime_t transmissionDelay) override;
    virtual void messageSendHop(cMessage *msg, cGate *srcGate) override;
    virtual void messageSendHop(cMessage *msg, cGate *srcGate, simtime_t propagationDelay, simtime_t transmissionDelay, bool discard) override;
    virtual void endSend(cMessage *msg) override;
    virtual void messageCreated(cMessage *msg) override;
    virtual void messageCloned(cMessage *msg, cMessage *clone) override;
    virtual void messageDeleted(cMessage *msg) override;
    virtual void moduleReparented(cModule *module, cModule *oldparent, int oldId) override;
    virtual void componentMethodBegin(cComponent *from, cComponent *to, const char *methodFmt, va_list va) override;
    virtual void componentMethodEnd() override;
    virtual void moduleCreated(cModule *newmodule) override;
    virtual void moduleDeleted(cModule *module) override;
    virtual void gateCreated(cGate *newgate) override;
    virtual void gateDeleted(cGate *gate) override;
    virtual void connectionCreated(cGate *srcgate) override;
    virtual void connectionDeleted(cGate *srcgate) override;
    virtual void displayStringChanged(cComponent *component) override;
    virtual void logLine(const char *prefix, const char *line, int lineLength) override;
    virtual void stoppedWithException(bool isError, int resultCode, const char *message) override;
    //@}

  private:
    void clearInternalState();

    /** @name Keyframe functions */
    //@{
    void addSimulationStateEventLogEntry(EventLogEntryReference reference);
    void addSimulationStateEventLogEntry(eventnumber_t eventNumber, int entryIndex);
    void removeSimulationStateEventLogEntry(EventLogEntryReference reference);
    void removeSimulationStateEventLogEntry(eventnumber_t eventNumber, int entryIndex);

    void removeBeginSendEntryReference(cMessage *message);
    void recordKeyframe();
    void addPreviousEventNumber(eventnumber_t previousEventNumber);
    //@}
};

} // namespace envir
}  // namespace omnetpp

#endif
