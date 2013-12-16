//==========================================================================
//  EVENTLOGFILEMGR.H - part of
//                     OMNeT++/OMNEST
//            Discrete System Simulation in C++
//
//  Author: Andras Varga, Levente Meszaros
//
//==========================================================================

/*--------------------------------------------------------------*
  Copyright (C) 1992-2008 Andras Varga
  Copyright (C) 2006-2008 OpenSim Ltd.

  This file is distributed WITHOUT ANY WARRANTY. See the file
  `license' for details on this and other legal matters.
*--------------------------------------------------------------*/

#ifndef __EVENTLOGFILEMGR_H
#define __EVENTLOGFILEMGR_H

#include "simkerneldefs.h"
#include "envirdefs.h"
#include "opp_string.h"
#include "objectprinter.h"
#include "intervals.h"

NAMESPACE_BEGIN

class cComponent;
class cModule;
class cChannel;
class cMessage;
class cGate;

/**
 * Responsible for writing the eventlog file.
 */
class ENVIR_API EventlogFileManager
{
  private:
    opp_string filename;
    FILE *feventlog;
    ObjectPrinter *objectPrinter;
    Intervals *recordingIntervals;
    eventnumber_t eventNumber;
    int entryIndex;
    int keyframeBlockSize;
    file_offset_t previousKeyframeFileOffset;
    bool isEventLogRecordingEnabled;
    bool isModuleEventLogRecordingEnabled;
    bool isIntervalEventLogRecordingEnabled;

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
                fprintf(file, "%" INT64_PRINTF_FORMAT "d:%d", eventNumber, beginEntryIndex);
            else
                fprintf(file, "%" INT64_PRINTF_FORMAT "d:%d-%d", eventNumber, beginEntryIndex, endEntryIndex);
        }
    };

    // keyframe state
    std::vector<eventnumber_t> consequenceLookaheadLimits;
    std::map<eventnumber_t, std::vector<EventLogEntryRange> > eventNumberToSimulationStateEventLogEntryRanges;
    std::map<cModule *, EventLogEntryReference> moduleToModuleDisplayStringChangedEntryReferenceMap;
    std::map<cChannel *, EventLogEntryReference> channelToConnectionDisplayStringChangedEntryReferenceMap;
    std::map<cMessage *, EventLogEntryReference> messageToBeginSendEntryReferenceMap;

  public:
    EventlogFileManager();
    virtual ~EventlogFileManager();

    virtual void configure();
    virtual void open();
    virtual void close();
    virtual void remove();
    virtual void startRun();
    virtual void endRun(bool isError, int resultCode, const char *message);

    virtual bool hasRecordingIntervals() const;
    virtual void clearRecordingIntervals();

    virtual void recordSimulation();
    virtual void recordInitialize();
    virtual void recordMessages();
    virtual void recordModules(cModule *module);
    virtual void recordConnections(cModule *module);

    virtual void flush();

    /** @name Functions called from cEnvir's similar functions */
    //@{
    virtual void simulationEvent(cMessage *msg);
    virtual void bubble(cComponent *component, const char *text);
    virtual void messageScheduled(cMessage *msg);
    virtual void messageCancelled(cMessage *msg);
    virtual void beginSend(cMessage *msg);
    virtual void messageSendDirect(cMessage *msg, cGate *toGate, simtime_t propagationDelay, simtime_t transmissionDelay);
    virtual void messageSendHop(cMessage *msg, cGate *srcGate);
    virtual void messageSendHop(cMessage *msg, cGate *srcGate, simtime_t propagationDelay, simtime_t transmissionDelay);
    virtual void endSend(cMessage *msg);
    virtual void messageCreated(cMessage *msg);
    virtual void messageCloned(cMessage *msg, cMessage *clone);
    virtual void messageDeleted(cMessage *msg);
    virtual void moduleReparented(cModule *module, cModule *oldparent);
    virtual void componentMethodBegin(cComponent *from, cComponent *to, const char *methodFmt, va_list va);
    virtual void componentMethodEnd();
    virtual void moduleCreated(cModule *newmodule);
    virtual void moduleDeleted(cModule *module);
    virtual void gateCreated(cGate *newgate);
    virtual void gateDeleted(cGate *gate);
    virtual void connectionCreated(cGate *srcgate);
    virtual void connectionDeleted(cGate *srcgate);
    virtual void displayStringChanged(cComponent *component);
    virtual void sputn(const char *s, int n);
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

NAMESPACE_END

#endif
