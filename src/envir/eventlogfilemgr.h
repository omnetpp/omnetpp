//==========================================================================
//  EVENTLOGFILEMGR.H - part of
//                     OMNeT++/OMNEST
//            Discrete System Simulation in C++
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

#include <sstream>
#include "common/filelock.h"
#include "omnetpp/simkerneldefs.h"
#include "omnetpp/opp_string.h"
#include "omnetpp/envirext.h"
#include "omnetpp/cobject.h"
#include "omnetpp/opp_string.h"
#include "envirdefs.h"
#include "objectprinter.h"
#include "intervals.h"

namespace omnetpp {

#define EVENTLOG_VERSION 2

class cComponent;
class cModule;
class cChannel;

namespace envir {

/**
 * Responsible for writing the eventlog file. The file format is line oriented,
 * each line contains exactly one eventlog entry. Snapshots and index entries
 * are written periodically to be able to read large eventlog files efficiently.
 */
class ENVIR_API EventlogFileManager : public cIEventlogManager, public cISimulationLifecycleListener
{
  private:
    enum ChunkType {
        NONE,
        BEGIN,
        END,
        EVENT,
        INDEX,
        SNAPSHOT
    };

    // index data structures
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

        void print(std::ostream& stream)
        {
            if (beginEntryIndex == endEntryIndex)
                stream << eventNumber << ":" << beginEntryIndex;
            else
                stream << eventNumber << ":" << beginEntryIndex << "-" << endEntryIndex;
        }
    };

  private:
    cEnvir *envir = nullptr;
    std::string filename;

    // configuration that does not change over time
    bool recordEventLog = false;  // value of the CFGID_RECORD_EVENTLOG config option
    int64_t maxSize = -1;
    int64_t minTruncatedSize = -1;
    int64_t snapshotFrequency = -1;
    int64_t indexFrequency = -1;
    ObjectPrinter *messageDetailPrinter = nullptr;

    // internal state
    FILE *feventlog = nullptr;
    common::FileLock *fileLock = nullptr;
    Intervals *recordingIntervals = nullptr;

    ChunkType lastChunk = NONE;
    eventnumber_t eventNumber = -1; // last written eventNumber
    simtime_t simulationTime = -1; // last written simulationTime
    int entryIndex = -1;

    file_offset_t beginningFileOffset = -1; // virtual file offset of the very beginning
    file_offset_t previousIndexFileOffset = -1; // virtual file offset
    file_offset_t previousSnapshotFileOffset = -1; // virtual file offset

    // general recording flags
    bool isRecordingEnabled = true;  // whether eventlog recording is currently enabled (recording turned on, and not currently suspended)
    bool isEventRecordingEnabled = true;  // whether the current event should be recorded (recording is enabled, module and interval filters match, etc)

    // recording flags for filters
    bool isModuleFilterRecordingEnabled = true; // module filter matches the current event
    bool isIntervalFilterRecordingEnabled = true; // interval filter matches the current event

    // recording flags based on entry kind
    bool isTextRecordingEnabled = true;
    bool isMessageRecordingEnabled = true;
    bool isModuleRecordingEnabled = true;
    bool isMethodCallRecordingEnabled = true;
    bool isDisplayStringRecordingEnabled = true;
    bool isCustomRecordingEnabled = true;

    /**
     * List of entry ranges present in the snapshot.
     */
    std::map<eventnumber_t, std::vector<EventLogEntryRange> > eventNumberToSnapshotEventLogEntryRanges;

    /**
     * List of entry ranges marked to be added to the index.
     */
    std::map<eventnumber_t, std::vector<EventLogEntryRange> > eventNumberToAddedIndexEventLogEntryRanges;

    /**
     * List of entry ranges marked to be removed from the index.
     */
    std::map<eventnumber_t, std::vector<EventLogEntryRange> > eventNumberToRemovedIndexEventLogEntryRanges;

    // index helper state
    std::map<cModule *, EventLogEntryReference> moduleToModuleCreatedEntryReferenceMap;
    std::map<cModule *, EventLogEntryReference> moduleToModuleDisplayStringChangedEntryReferenceMap;
    std::map<cGate *, EventLogEntryReference> channelToConnectionCreatedEntryReferenceMap;
    std::map<cGate *, EventLogEntryReference> channelToConnectionDisplayStringChangedEntryReferenceMap;
    std::map<cGate *, EventLogEntryReference> gateToGateCreatedEntryReferenceMap;
    // TODO: std::map<cGate *, EventLogEntryReference> gateToGateDisplayStringChangedEntryReferenceMap;
    std::map<cMessage *, EventLogEntryReference> messageToEntryReferenceMap;
    // TODO: std::map<cMessage *, EventLogEntryReference> messageToMessageDisplayStringChangedEntryReferenceMap;

  protected:
    /**
     * A cISimulationLifecycleListener method.
     */
    virtual void lifecycleEvent(SimulationLifecycleEventType eventType, cObject *details) override;

  public:
    /**
     * The default constructor doesn't fully initialize the eventlog manager.
     */
    explicit EventlogFileManager();

    /**
     * Destructor.
     */
    virtual ~EventlogFileManager();

    /**
     * Returns the eventlog filename.
     */
    virtual const char *getFileName() const override { return filename.c_str(); }

    /**
     * Initializes the eventlog manager according to the configuration.
     * This function must be called before any other.
     */
    virtual void configure();

    /**
     * Returns true if the file is open.
     */
    virtual bool isOpen() { return feventlog != nullptr; }

    /**
     * Opens the eventlog file for writing. If the file exists it will be deleted.
     */
    virtual void open();

    /**
     * Closes the eventlog file. Makes sure that the file ends with an index.
     */
    virtual void close();

    /**
     * Deletes the eventlog file. The eventlog must be closed before calling
     * this function.
     */
    virtual void remove();

    /**
     * Truncates the eventlog file according to the configuration. It writes a
     * new file, deletes the old one and renames the new to the old name. The
     * new eventlog file will start with the same simulation begin entry. The
     * rest of the file is copied from the old one starting at a snapshot.
     */
    virtual void truncate();

    /**
     * Temporarily suspends eventlog recording. Makes sure that the file ends
     * with an index.
     */
    virtual void suspend() override;

    /**
     * Resumes temporarily suspended eventlog recording. Records a snapshot if
     * some events were processed since the last suspend.
     */
    virtual void resume() override;

    /**
     * Forces writing out all buffered output to the eventlog file.
     */
    virtual void flush() override;

    virtual void startRun();
    virtual void endRun(bool isError, int resultCode, const char *message) override;

    virtual bool hasRecordingIntervals() const;
    virtual void clearRecordingIntervals();

    /** @name Functions called from cEnvir's similar functions */
    //@{
    virtual void simulationEvent(cEvent *event) override;
    virtual void bubble(cComponent *component, const char *text) override;
    virtual void messageScheduled(cMessage *msg) override;
    virtual void messageCancelled(cMessage *msg) override;
    virtual void beginSend(cMessage *msg, const SendOptions& options) override;
    virtual void messageSendDirect(cMessage *msg, cGate *toGate, const ChannelResult& result) override;
    virtual void messageSendHop(cMessage *msg, cGate *srcGate) override;
    virtual void messageSendHop(cMessage *msg, cGate *srcGate, const ChannelResult& result) override;
    virtual void endSend(cMessage *msg) override;
    virtual void messageCreated(cMessage *msg) override;
    virtual void messageCloned(cMessage *msg, cMessage *clone) override;
    virtual void messageDeleted(cMessage *msg) override;
    virtual void moduleReparented(cModule *module, cModule *oldparent, int oldId) override { throw opp_runtime_error("Unsupported operation"); }
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

    /** @name Record functions */
    //@{
    void recordSimulationBegin();
    void recordSimulationEnd(bool isError, int resultCode, const char *message);
    void recordInitialize();
    void recordSnapshot();
    void recordModules(cModule *module);
    void recordConnections(cModule *module);
    void recordMessages(cObject *owner);
    void recordMessage(cMessage *msg);
    void recordIndex();
    //@}

    /** @name Index functions */
    //@{
    void addIndexEventLogEntry(EventLogEntryReference reference);
    void addIndexEventLogEntry(eventnumber_t eventNumber, int entryIndex);
    bool addIndexEventLogEntry(std::map<eventnumber_t, std::vector<EventLogEntryRange> >& eventNumberToIndexEventLogEntryRanges, eventnumber_t eventNumber, int entryIndex);
    void removeIndexEventLogEntry(EventLogEntryReference reference);
    void removeIndexEventLogEntry(eventnumber_t eventNumber, int entryIndex);
    bool removeIndexEventLogEntry(std::map<eventnumber_t, std::vector<EventLogEntryRange> >& eventNumberToIndexEventLogEntryRanges, eventnumber_t eventNumber, int entryIndex);

    void addMessageEntryReference(cMessage *msg);
    void removeMessageEntryReference(cMessage *msg);
    //@}

    file_offset_t toRealFileOffset(file_offset_t virtualFileOffset) const { return virtualFileOffset - beginningFileOffset; }
    file_offset_t toVirtualFileOffset(file_offset_t realFileOffset) const { return realFileOffset + beginningFileOffset; }
};

}  // namespace envir
}  // namespace omnetpp

#endif
