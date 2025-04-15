//==========================================================================
//  EVENTLOGFILEMGR.CC - part of
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

#include <algorithm>
#include "common/opp_ctype.h"
#include "common/commonutil.h"  // vsnprintf
#include "common/fileutil.h"
#include "common/filelock.h"
#include "common/stringtokenizer.h"
#include "omnetpp/cconfigoption.h"
#include "omnetpp/cconfiguration.h"
#include "omnetpp/cmodule.h"
#include "omnetpp/cfutureeventset.h"
#include "omnetpp/cpacket.h"
#include "omnetpp/cgate.h"
#include "omnetpp/cchannel.h"
#include "omnetpp/csimplemodule.h"
#include "omnetpp/cdisplaystring.h"
#include "omnetpp/cclassdescriptor.h"
#include "omnetpp/cfutureeventset.h"
#include "omnetpp/cfingerprint.h"
#include "omnetpp/cenvir.h"
#include "omnetpp/checkandcast.h"
#include "eventlogfilemgr.h"
#include "eventlogwriter.h"
#include "envirbase.h"

using namespace omnetpp::common;

namespace omnetpp {
namespace envir {

Register_Class(EventlogFileManager)

Register_PerRunConfigOption(CFGID_EVENTLOG_FILE, "eventlog-file", CFG_FILENAME, "${resultdir}/${configname}-${iterationvarsf}#${repetition}.elog", "Name of the eventlog file to generate.");
Register_PerRunConfigOptionU(CFGID_EVENTLOG_MAX_SIZE, "eventlog-max-size", "B", "10 GiB", "Specify the maximum size of the eventlog file in bytes. The eventlog file is automatically truncated when this limit is reached.");
Register_PerRunConfigOptionU(CFGID_EVENTLOG_MIN_TRUNCATED_SIZE, "eventlog-min-truncated-size", "B", "1 GiB", "Specify the minimum size of the eventlog file in bytes after the file is truncated. Truncation means older events are discarded while newer ones are kept.");
Register_PerRunConfigOptionU(CFGID_EVENTLOG_SNAPSHOT_FREQUENCY, "eventlog-snapshot-frequency", "B", "100 MiB", "The eventlog file contains periodically recorded snapshots. Each one describes the complete simulation state at a specific event. Snapshots help various tools to handle large eventlog files more efficiently. Specifying greater value means less help, while smaller value means bigger eventlog files.");
Register_PerRunConfigOptionU(CFGID_EVENTLOG_INDEX_FREQUENCY, "eventlog-index-frequency", "B", "1 MiB", "The eventlog file contains incremental snapshots called index. An index is much smaller than a full snapshot, but it only contains the differences since the last index.");
Register_PerRunConfigOption(CFGID_EVENTLOG_OPTIONS, "eventlog-options", CFG_CUSTOM, nullptr, "Allows for reducing the size of the eventlog file by recording only specific types of content. Specify a comma-separated subset of the following keywords: text, message, module, methodcall, displaystring and custom. By default, all categories are enabled.");
Register_PerRunConfigOption(CFGID_EVENTLOG_MESSAGE_DETAIL_PATTERN, "eventlog-message-detail-pattern", CFG_CUSTOM, nullptr,
        "A list of patterns separated by '|' character which will be used to write "
        "message detail information into the eventlog for each message sent during "
        "the simulation. The message detail will be presented in the sequence chart "
        "tool. Each pattern starts with an object pattern optionally followed by ':' "
        "character and a comma separated list of field patterns. In both "
        "patterns and/or/not/* and various field match expressions can be used. "
        "The object pattern matches to class name, the field pattern matches to field name by default.\n"
        "  `EVENTLOG-MESSAGE-DETAIL-PATTERN := ( DETAIL-PATTERN '|' )* DETAIL_PATTERN`\n"
        "  `DETAIL-PATTERN := OBJECT-PATTERN [ ':' FIELD-PATTERNS ]`\n"
        "  `OBJECT-PATTERN := MATCH-EXPRESSION`\n"
        "  `FIELD-PATTERNS := ( FIELD-PATTERN ',' )* FIELD_PATTERN`\n"
        "  `FIELD-PATTERN := MATCH-EXPRESSION`\n"
        "Examples:\n"
        "  `*`: captures all fields of all messages\n"
        "  `*Frame:*Address,*Id`: captures all fields named somethingAddress and somethingId from messages of any class named somethingFrame\n"
        "  `MyMessage:declaredOn=~MyMessage`: captures instances of MyMessage recording the fields declared on the MyMessage class\n"
        "  `*:(not declaredOn=~cMessage and not declaredOn=~cNamedObject and not declaredOn=~cObject)`: records user-defined fields from all messages");
Register_PerRunConfigOption(CFGID_EVENTLOG_RECORDING_INTERVALS, "eventlog-recording-intervals", CFG_CUSTOM, nullptr, "Simulation time interval(s) when events should be recorded. Syntax: `[<from>]..[<to>],...` That is, both start and end of an interval are optional, and intervals are separated by comma. Example: `..10.2, 22.2..100, 233.3..`");
Register_PerObjectConfigOption(CFGID_MODULE_EVENTLOG_RECORDING, "module-eventlog-recording", KIND_SIMPLE_MODULE, CFG_BOOL, "true", "Enables recording events on a per module basis. This is meaningful for simple modules only. Usage: `<module-full-path>.module-eventlog-recording=true/false`. Examples: `**.router[10..20].**.module-eventlog-recording = true`; `**.module-eventlog-recording = false`");

extern cConfigOption *CFGID_RECORD_EVENTLOG;

static bool compareMessageEventNumbers(cMessage *message1, cMessage *message2)
{
    return message1->getPreviousEventNumber() < message2->getPreviousEventNumber();
}

static ObjectPrinterRecursionControl recurseIntoMessageFields(any_ptr object, cClassDescriptor *descriptor, int fieldIndex, any_ptr fieldValue, any_ptr *parents, int level)
{
    const char *propertyValue = descriptor->getFieldProperty(fieldIndex, "eventlog");

    if (propertyValue) {
        if (!strcmp(propertyValue, "skip"))
            return SKIP;
        else if (!strcmp(propertyValue, "fullName"))
            return FULL_NAME;
        else if (!strcmp(propertyValue, "fullPath"))
            return FULL_PATH;
    }

    bool isCObject = descriptor->getFieldIsCObject(fieldIndex);
    if (!isCObject)
        return RECURSE;
    else {
        if (fieldValue != nullptr)
            return RECURSE;
        else {
            cArray *array = check_and_cast_nullable<cArray*>(fromAnyPtr<cObject>(fieldValue));
            return !array || array->size() != 0 ? RECURSE : SKIP;
        }
    }
}

EventlogFileManager::EventlogFileManager()
{
    envir = getEnvir();
    envir->addLifecycleListener(this);
    clearInternalState();
}

EventlogFileManager::~EventlogFileManager()
{
    delete messageDetailPrinter;
    messageDetailPrinter = nullptr;
    delete recordingIntervals;
    recordingIntervals = nullptr;
    delete fileLock;
    fileLock = nullptr;
}

void EventlogFileManager::clearInternalState()
{
    lastChunk = NONE;
    eventNumber = -1;
    simulationTime = -1;
    entryIndex = -1;
    beginningFileOffset = -1;
    previousIndexFileOffset = -1;
    previousSnapshotFileOffset = -1;
    isEventRecordingEnabled = true;
    isIntervalFilterRecordingEnabled = true;
    isModuleFilterRecordingEnabled = true;
    isRecordingEnabled = recordEventLog;
    eventNumberToSnapshotEventLogEntryRanges.clear();
    eventNumberToAddedIndexEventLogEntryRanges.clear();
    eventNumberToRemovedIndexEventLogEntryRanges.clear();
    moduleToModuleCreatedEntryReferenceMap.clear();
    moduleToModuleDisplayStringChangedEntryReferenceMap.clear();
    channelToConnectionCreatedEntryReferenceMap.clear();
    channelToConnectionDisplayStringChangedEntryReferenceMap.clear();
    gateToGateCreatedEntryReferenceMap.clear();
    messageToEntryReferenceMap.clear();
    configureRecordingOptions();
}

void EventlogFileManager::configure()
{
    recordEventLog = envir->getConfig()->getAsBool(CFGID_RECORD_EVENTLOG);

    // setup eventlog object printer
    delete messageDetailPrinter;
    messageDetailPrinter = nullptr;
    const char *eventLogMessageDetailPattern = envir->getConfig()->getAsCustom(CFGID_EVENTLOG_MESSAGE_DETAIL_PATTERN);
    if (eventLogMessageDetailPattern)
        messageDetailPrinter = new ObjectPrinter(recurseIntoMessageFields, eventLogMessageDetailPattern, 3);

    configureRecordingOptions();

    // set up recording intervals
    delete recordingIntervals;
    recordingIntervals = new Intervals();
    const char *text = envir->getConfig()->getAsCustom(CFGID_EVENTLOG_RECORDING_INTERVALS);
    if (text)
        recordingIntervals->parse(text);

    // setup filename
    filename = envir->getConfig()->getAsFilename(CFGID_EVENTLOG_FILE);
    dynamic_cast<EnvirBase *>(envir)->processFileName(filename);

    // file limits
    maxSize = envir->getConfig()->getAsDouble(CFGID_EVENTLOG_MAX_SIZE);
    minTruncatedSize = envir->getConfig()->getAsDouble(CFGID_EVENTLOG_MIN_TRUNCATED_SIZE);
    snapshotFrequency = envir->getConfig()->getAsDouble(CFGID_EVENTLOG_SNAPSHOT_FREQUENCY);
    indexFrequency = envir->getConfig()->getAsDouble(CFGID_EVENTLOG_INDEX_FREQUENCY);
}

void EventlogFileManager::configureRecordingOptions()
{
    const char *eventLogOptions = envir->getConfig()->getAsCustom(CFGID_EVENTLOG_OPTIONS);
    if (eventLogOptions) {
        isTextRecordingEnabled = isMessageRecordingEnabled = isModuleRecordingEnabled = isMethodCallRecordingEnabled = isDisplayStringRecordingEnabled = isCustomRecordingEnabled = false;
        common::StringTokenizer tokenizer(eventLogOptions, ", ");
        while (tokenizer.hasMoreTokens()) {
            const char *token = tokenizer.nextToken();
            if (!strcmp(token, "text")) isTextRecordingEnabled = true;
            else if (!strcmp(token, "message")) isMessageRecordingEnabled = true;
            else if (!strcmp(token, "module")) isModuleRecordingEnabled = true;
            else if (!strcmp(token, "methodcall")) isMethodCallRecordingEnabled = true;
            else if (!strcmp(token, "displaystring")) isDisplayStringRecordingEnabled = true;
            else if (!strcmp(token, "custom")) isCustomRecordingEnabled = true;
            else throw opp_runtime_error("Unknown eventlog-options parameter value `%s", token);
        }
    }
    else {
        isTextRecordingEnabled = true;
        isMessageRecordingEnabled = true;
        isModuleRecordingEnabled = true;
        isMethodCallRecordingEnabled = true;
        isDisplayStringRecordingEnabled = true;
        isCustomRecordingEnabled = true;
    }
}

void EventlogFileManager::lifecycleEvent(SimulationLifecycleEventType eventType, cObject *details)
{
    switch (eventType) {
        case LF_PRE_NETWORK_SETUP:
            configure();
            if (recordEventLog) {
                ASSERT(!isOpen());
                open();
                startRun();
                flush();
            }
            else
                remove();
            break;
        case LF_ON_RUN_END:
            if (isOpen()) {
                if (isEventRecordingEnabled)
                    recordSimulationEnd(false, -1, "");
                close();
            }
            break;
        case LF_ON_SIMULATION_PAUSE:
            flush();
            break;
        default:
            break;
    }
}

void EventlogFileManager::open()
{
    mkPath(directoryOf(filename.c_str()).c_str());
    feventlog = fopen(filename.c_str(), "w+b");
    if (!feventlog)
        throw opp_runtime_error("Cannot open eventlog file `%s' for write", filename.c_str());
    printf("Recording eventlog to file `%s'...\n", filename.c_str());
    fileLock = new FileLock(feventlog, filename.c_str());
    clearInternalState();
}

void EventlogFileManager::close()
{
    ASSERT(feventlog);
    fclose(feventlog);
    feventlog = nullptr;
    isEventRecordingEnabled = false;
    delete fileLock;
    fileLock = nullptr;
}

void EventlogFileManager::remove()
{
    removeFile(filename.c_str(), "old eventlog file");
    entryIndex = -1;
}

void EventlogFileManager::truncate()
{
    size_t readSize;
    char buffer[BUFSIZ];
    // acquire an exclusive lock to prevent reading the file while it is being truncated
    FileLockAcquirer fileLockAcquirer(fileLock, FILE_LOCK_EXCLUSIVE);
    // linear search backwards until we reach the first snapshot entry
    // if no snapshot entry found then truncation is skipped
    opp_fseek(feventlog, 0, SEEK_END);
    if (ferror(feventlog))
        throw opp_runtime_error("Cannot seek in file '%s', error code %d", filename.c_str(), ferror(feventlog));
    bool foundSnapshot = false;
    file_offset_t oldFileSize = opp_ftell(feventlog);
    file_offset_t copyFromOffset = oldFileSize - minTruncatedSize;
    while (copyFromOffset > snapshotFrequency) {
        copyFromOffset = std::max((file_offset_t)0, copyFromOffset - BUFSIZ);
        opp_fseek(feventlog, copyFromOffset, SEEK_SET);
        if (ferror(feventlog))
            throw opp_runtime_error("Cannot seek in file '%s', error code %d", filename.c_str(), ferror(feventlog));
        readSize = fread(buffer, 1, BUFSIZ, feventlog);
        if (ferror(feventlog))
            throw opp_runtime_error("Read error in file '%s', error code %d", filename.c_str(), ferror(feventlog));
        const char *match = opp_strnistr(buffer, "\nS ", readSize, true);
        if (match) {
            foundSnapshot = true;
            copyFromOffset += (file_offset_t)(match - buffer);
            break;
        }
    }
    // truncate only if a snapshot entry is found
    if (foundSnapshot) {
        // write begin simulation entry to the very beginning
        const char *runId = envir->getConfigEx()->getVariable(CFGVAR_RUNID);
        beginningFileOffset = toVirtualFileOffset(copyFromOffset);
        opp_fseek(feventlog, 0, SEEK_SET);
        if (ferror(feventlog))
            throw opp_runtime_error("Cannot seek in file '%s', error code %d", filename.c_str(), ferror(feventlog));
        EventLogWriter::recordSimulationBeginEntry_ov_ev_rid(feventlog, OMNETPP_VERSION, EVENTLOG_VERSION, runId);
        file_offset_t copyToOffset = opp_ftell(feventlog);
        beginningFileOffset -= copyToOffset;
        // copy the trailing content of the eventlog file backwards
        // TODO: shall we use a bigger (say 1 MB) buffer for performance reasons?
        do {
            opp_fseek(feventlog, copyFromOffset, SEEK_SET);
            if (ferror(feventlog))
                throw opp_runtime_error("Cannot seek in file '%s', error code %d", filename.c_str(), ferror(feventlog));
            readSize = fread(buffer, 1, BUFSIZ, feventlog);
            if (ferror(feventlog))
                throw opp_runtime_error("Read error in file '%s', error code %d", filename.c_str(), ferror(feventlog));
            copyFromOffset += readSize;
            if (readSize) {
                opp_fseek(feventlog, copyToOffset, SEEK_SET);
                if (ferror(feventlog))
                    throw opp_runtime_error("Cannot seek in file '%s', error code %d", filename.c_str(), ferror(feventlog));
                fwrite(buffer, 1, readSize, feventlog);
                if (ferror(feventlog))
                    throw opp_runtime_error("Write error in file '%s', error code %d", filename.c_str(), ferror(feventlog));
                copyToOffset += readSize;
            }
        }
        while (readSize);
        // actually truncate the eventlog file and seek to the end for further appending
        opp_ftruncate(fileno(feventlog), copyToOffset);
    }
    // seek to end to continue with appending the eventlog
    opp_fseek(feventlog, 0, SEEK_END);
    if (ferror(feventlog))
        throw opp_runtime_error("Cannot seek in file '%s', error code %d", filename.c_str(), ferror(feventlog));
}

void EventlogFileManager::suspend()
{
    ASSERT(isRecordingEnabled);
    isRecordingEnabled = false;
    if (hasRecordingIntervals())
        clearRecordingIntervals();
}

void EventlogFileManager::resume()
{
    ASSERT(!isRecordingEnabled);
    if (!isOpen()) {
        open();
        recordSimulationBegin();
        if (hasRecordingIntervals())
            clearRecordingIntervals();
    }
    if (lastChunk != SNAPSHOT && (eventNumber == -1 || eventNumber != cSimulation::getActiveSimulation()->getEventNumber() - 1))
        recordSnapshot();
    isRecordingEnabled = true;
}

void EventlogFileManager::startRun()
{
    if (isEventRecordingEnabled) {
        recordSimulationBegin();
        recordInitialize();
    }
}

void EventlogFileManager::endRun(bool isError, int resultCode, const char *message)
{
    if (isOpen()) {   // note: an earlier error might have prevented the file from being opened
        if (isEventRecordingEnabled)
            recordSimulationEnd(isError, resultCode, message);
        close();
    }
}

bool EventlogFileManager::hasRecordingIntervals() const
{
    return recordingIntervals && !recordingIntervals->empty();
}

void EventlogFileManager::clearRecordingIntervals()
{
    if (recordingIntervals) {
        delete recordingIntervals;
        recordingIntervals = nullptr;
        envir->printfmsg("Switching to manual control of eventlog recording -- the recording intervals specified in the configuration will be ignored.");
    }
}

void EventlogFileManager::flush()
{
    if (isEventRecordingEnabled)
        fflush(feventlog);
}

void EventlogFileManager::simulationEvent(cEvent *event)
{
    cMessage *msg = dynamic_cast<cMessage*>(event);
    cModule *mod = msg ? msg->getArrivalModule() : nullptr;
    isModuleFilterRecordingEnabled = mod ? mod->isRecordEvents() : true;
    isIntervalFilterRecordingEnabled = !recordingIntervals || recordingIntervals->contains(cSimulation::getActiveSimulation()->getSimTime());
    isEventRecordingEnabled = isRecordingEnabled && isModuleFilterRecordingEnabled && isIntervalFilterRecordingEnabled;
    if (isEventRecordingEnabled) {
        eventNumber = cSimulation::getActiveSimulation()->getEventNumber();
        simulationTime = cSimulation::getActiveSimulation()->getSimTime();
        FileLockAcquirer fileLockAcquirer(fileLock, FILE_LOCK_EXCLUSIVE);
        file_offset_t fileOffset = opp_ftell(feventlog);
        if (lastChunk != INDEX && fileOffset - toRealFileOffset(previousIndexFileOffset) > indexFrequency)
            recordIndex();
        if (lastChunk != SNAPSHOT && fileOffset - toRealFileOffset(previousSnapshotFileOffset) > snapshotFrequency) {
            if (lastChunk != INDEX)
                recordIndex();
            recordSnapshot();
        }
        fileOffset = opp_ftell(feventlog);
        if (fileOffset > maxSize)
            truncate();
        fprintf(feventlog, "\n");
        auto fingerprintCalculator = cSimulation::getActiveSimulation()->getFingerprintCalculator();
        if (msg)
            EventLogWriter::recordEventEntry_e_t_m_ce_msg_f(feventlog, eventNumber, cSimulation::getActiveSimulation()->getSimTime(), mod->getId(), msg->getPreviousEventNumber(), msg->getId(), (fingerprintCalculator ? fingerprintCalculator->str().c_str() : nullptr));
        else
            ; // TODO: record non message handling events
        entryIndex = 0;
        lastChunk = EVENT;
        if (msg) {
            removeMessageEntryReference(msg);
            addMessageEntryReference(msg);
        }
    }
    else {
        // TODO record non-message event
    }
}

void EventlogFileManager::bubble(cComponent *component, const char *text)
{
    if (isEventRecordingEnabled && isTextRecordingEnabled) {
        if (dynamic_cast<cModule *>(component)) {
            FileLockAcquirer fileLockAcquirer(fileLock, FILE_LOCK_EXCLUSIVE);
            cModule *mod = (cModule *)component;
            EventLogWriter::recordBubbleEntry_id_txt(feventlog, mod->getId(), text);
            entryIndex++;
        }
        else if (cChannel *channel = dynamic_cast<cChannel *>(component)) {
            // TODO
            (void)channel;
        }
    }
}

void EventlogFileManager::beginSend(cMessage *msg, const SendOptions& options)
{
    if (isEventRecordingEnabled && isMessageRecordingEnabled) {
        FileLockAcquirer fileLockAcquirer(fileLock, FILE_LOCK_EXCLUSIVE);
        cModule *ownerModule = dynamic_cast<cModule *>(msg->getOwner());
        bool isScheduled = msg->isScheduled();
        bool isPacket = msg->isPacket();
        cPacket *pkt = isPacket ? (cPacket *)msg : nullptr; // note: simply `(cPacket *)msg` would cause sanitizer to complain about illegal cast
        EventLogWriter::recordBeginSendEntry_id_tid_eid_etid_c_n_k_p_l_er_m_sm_sg_st_am_ag_at_d_pe_sd_up_tx(feventlog,
            msg->getId(), msg->getTreeId(), isPacket ? pkt->getEncapsulationId() : msg->getId(), isPacket ? pkt->getEncapsulationTreeId() : msg->getTreeId(),
            msg->getClassName(), msg->getFullName(),
            msg->getKind(), msg->getSchedulingPriority(), isPacket ? pkt->getBitLength() : 0, isPacket ? pkt->hasBitError() : false,
            ownerModule ? ownerModule->getId() : -1,
            isScheduled ? msg->getSenderModuleId() : -1, isScheduled ? msg->getSenderGateId() : -1, isScheduled ? msg->getSendingTime() : -1,
            isScheduled ? msg->getArrivalModuleId() : -1, isScheduled ? msg->getArrivalGateId() : -1, isScheduled ? msg->getArrivalTime() : -1,
            messageDetailPrinter ? messageDetailPrinter->printObjectToString(msg).c_str() : nullptr,
            msg->getPreviousEventNumber(),
            options.sendDelay,
            options.isUpdate,
            options.transmissionId_);
        entryIndex++;
        removeMessageEntryReference(msg);
        addMessageEntryReference(msg);
    }
}

void EventlogFileManager::messageScheduled(cMessage *msg)
{
    if (isEventRecordingEnabled && isMessageRecordingEnabled) {
        EventlogFileManager::beginSend(msg, SendOptions());
        EventlogFileManager::endSend(msg);
    }
}

void EventlogFileManager::messageCancelled(cMessage *msg)
{
    if (isEventRecordingEnabled && isMessageRecordingEnabled) {
        FileLockAcquirer fileLockAcquirer(fileLock, FILE_LOCK_EXCLUSIVE);
        cModule *ownerModule = dynamic_cast<cModule *>(msg->getOwner());
        bool isScheduled = msg->isScheduled();
        bool isPacket = msg->isPacket();
        cPacket *pkt = isPacket ? (cPacket *)msg : nullptr;
        EventLogWriter::recordCancelEventEntry_id_tid_eid_etid_c_n_k_p_l_er_m_sm_sg_st_am_ag_at_d_pe(feventlog,
            msg->getId(), msg->getTreeId(), isPacket ? pkt->getEncapsulationId() : msg->getId(), isPacket ? pkt->getEncapsulationTreeId() : msg->getTreeId(),
            msg->getClassName(), msg->getFullName(),
            msg->getKind(), msg->getSchedulingPriority(), isPacket ? pkt->getBitLength() : 0, isPacket ? pkt->hasBitError() : false,
            ownerModule ? ownerModule->getId() : -1,
            isScheduled ? msg->getSenderModuleId() : -1, isScheduled ? msg->getSenderGateId() : -1, isScheduled ? msg->getSendingTime() : -1,
            isScheduled ? msg->getArrivalModuleId() : -1, isScheduled ? msg->getArrivalGateId() : -1, isScheduled ? msg->getArrivalTime() : -1,
            messageDetailPrinter ? messageDetailPrinter->printObjectToString(msg).c_str() : nullptr,
            msg->getPreviousEventNumber());
        entryIndex++;
        removeMessageEntryReference(msg);
        addMessageEntryReference(msg);
    }
}

void EventlogFileManager::messageSendDirect(cMessage *msg, cGate *toGate, const ChannelResult& result)
{
    // TODO: store this related the message, so that we can repeat it in snapshots
    if (isEventRecordingEnabled && isMessageRecordingEnabled) {
        FileLockAcquirer fileLockAcquirer(fileLock, FILE_LOCK_EXCLUSIVE);
        ASSERT(result.remainingDuration >= 0);
        EventLogWriter::recordSendDirectEntry_sm_dm_dg_pd_td_rd(feventlog, msg->getSenderModuleId(), toGate->getOwnerModule()->getId(), toGate->getId(), result.delay, result.duration, result.remainingDuration);
        entryIndex++;
    }
}

void EventlogFileManager::messageSendHop(cMessage *msg, cGate *srcGate)
{
    // TODO: store this related the message, so that we can repeat it in snapshots
    if (isEventRecordingEnabled && isMessageRecordingEnabled) {
        FileLockAcquirer fileLockAcquirer(fileLock, FILE_LOCK_EXCLUSIVE);
        EventLogWriter::recordSendHopEntry_sm_sg(feventlog, srcGate->getOwnerModule()->getId(), srcGate->getId());
        entryIndex++;
    }
}

void EventlogFileManager::messageSendHop(cMessage *msg, cGate *srcGate, const ChannelResult& result)
{
    // TODO: store this related the message, so that we can repeat it in snapshots
    if (isEventRecordingEnabled && isMessageRecordingEnabled) {
        FileLockAcquirer fileLockAcquirer(fileLock, FILE_LOCK_EXCLUSIVE);
        ASSERT(result.remainingDuration >= 0);
        EventLogWriter::recordSendHopEntry_sm_sg_pd_td_rd_d(feventlog, srcGate->getOwnerModule()->getId(), srcGate->getId(), result.delay, result.duration, result.remainingDuration, result.discard);
        entryIndex++;
    }
}

void EventlogFileManager::endSend(cMessage *msg)
{
    // TODO: store this related the message, so that we can repeat it in snapshots
    if (isEventRecordingEnabled && isMessageRecordingEnabled) {
        FileLockAcquirer fileLockAcquirer(fileLock, FILE_LOCK_EXCLUSIVE);
        cModule *ownerModule = dynamic_cast<cModule *>(msg->getOwner());
        bool isScheduled = msg->isScheduled();
        bool isPacket = msg->isPacket();
        cPacket *pkt = isPacket ? (cPacket *)msg : nullptr;
        EventLogWriter::recordEndSendEntry_id_tid_eid_etid_c_n_k_p_l_er_m_sm_sg_st_am_ag_at_d_pe_i(feventlog,
            msg->getId(), msg->getTreeId(), isPacket ? pkt->getEncapsulationId() : msg->getId(), isPacket ? pkt->getEncapsulationTreeId() : msg->getTreeId(),
            msg->getClassName(), msg->getFullName(),
            msg->getKind(), msg->getSchedulingPriority(), isPacket ? pkt->getBitLength() : 0, isPacket ? pkt->hasBitError() : false,
            ownerModule ? ownerModule->getId() : -1,
            isScheduled ? msg->getSenderModuleId() : -1, isScheduled ? msg->getSenderGateId() : -1, isScheduled ? msg->getSendingTime() : -1,
            isScheduled ? msg->getArrivalModuleId() : -1, isScheduled ? msg->getArrivalGateId() : -1, isScheduled ? msg->getArrivalTime() : -1,
            messageDetailPrinter ? messageDetailPrinter->printObjectToString(msg).c_str() : nullptr,
            msg->getPreviousEventNumber(),
            msg->getArrivalGateId() != -1 ? msg->getArrivalGate()->getDeliverImmediately() : false);
        entryIndex++;
    }
}

void EventlogFileManager::messageCreated(cMessage *msg)
{
    if (isEventRecordingEnabled && isMessageRecordingEnabled) {
        FileLockAcquirer fileLockAcquirer(fileLock, FILE_LOCK_EXCLUSIVE);
        bool isPacket = msg->isPacket();
        cPacket *pkt = isPacket ? (cPacket *)msg : nullptr;
        EventLogWriter::recordCreateMessageEntry_id_tid_eid_etid_c_n_k_p_l_er_m_sm_sg_st_am_ag_at_d_pe(feventlog,
            msg->getId(), msg->getTreeId(), isPacket ? pkt->getEncapsulationId() : msg->getId(), isPacket ? pkt->getEncapsulationTreeId() : msg->getTreeId(),
            msg->getClassName(), msg->getFullName(),
            msg->getKind(), msg->getSchedulingPriority(), isPacket ? pkt->getBitLength() : 0, isPacket ? pkt->hasBitError() : false,
            -1, -1, -1, -1, -1, -1, -1,
            messageDetailPrinter ? messageDetailPrinter->printObjectToString(msg).c_str() : nullptr,
            msg->getPreviousEventNumber());
        entryIndex++;
        addMessageEntryReference(msg);
    }
}

void EventlogFileManager::messageCloned(cMessage *msg, cMessage *clone)
{
    if (isEventRecordingEnabled && isMessageRecordingEnabled) {
        FileLockAcquirer fileLockAcquirer(fileLock, FILE_LOCK_EXCLUSIVE);
        cModule *ownerModule = dynamic_cast<cModule *>(clone->getOwner());
        bool isScheduled = clone->isScheduled();
        bool isPacket = clone->isPacket();
        cPacket *pkt = isPacket ? (cPacket *)msg : nullptr;
        EventLogWriter::recordCloneMessageEntry_id_tid_eid_etid_c_n_k_p_l_er_m_sm_sg_st_am_ag_at_d_pe_cid(feventlog,
            clone->getId(), clone->getTreeId(), isPacket ? pkt->getEncapsulationId() : clone->getId(), isPacket ? pkt->getEncapsulationTreeId() : clone->getTreeId(),
            clone->getClassName(), clone->getFullName(),
            clone->getKind(), clone->getSchedulingPriority(), isPacket ? pkt->getBitLength() : 0, isPacket ? pkt->hasBitError() : false,
            ownerModule ? ownerModule->getId() : -1,
            isScheduled ? clone->getSenderModuleId() : -1, isScheduled ? clone->getSenderGateId() : -1, isScheduled ? clone->getSendingTime() : -1,
            isScheduled ? clone->getArrivalModuleId() : -1, isScheduled ? clone->getArrivalGateId() : -1, isScheduled ? clone->getArrivalTime() : -1,
            messageDetailPrinter ? messageDetailPrinter->printObjectToString(msg).c_str() : nullptr,
            clone->getPreviousEventNumber(), msg->getId());
        entryIndex++;
        removeMessageEntryReference(msg);
        addMessageEntryReference(msg);
        addMessageEntryReference(clone);
    }
}

void EventlogFileManager::messageDeleted(cMessage *msg)
{
    if (isEventRecordingEnabled && isMessageRecordingEnabled) {
        FileLockAcquirer fileLockAcquirer(fileLock, FILE_LOCK_EXCLUSIVE);
        cModule *ownerModule = dynamic_cast<cModule *>(msg->getOwner());
        bool isPacket = msg->isPacket();
        cPacket *pkt = isPacket ? (cPacket *)msg : nullptr;
        EventLogWriter::recordDeleteMessageEntry_id_tid_eid_etid_c_n_k_p_l_er_m_sm_sg_st_am_ag_at_d_pe(feventlog,
            msg->getId(), msg->getTreeId(), isPacket ? pkt->getEncapsulationId() : msg->getId(), isPacket ? pkt->getEncapsulationTreeId() : msg->getTreeId(),
            msg->getClassName(), msg->getFullName(),
            msg->getKind(), msg->getSchedulingPriority(), isPacket ? pkt->getBitLength() : 0, isPacket ? pkt->hasBitError() : false,
            ownerModule ? ownerModule->getId() : -1,
            -1, -1, -1, -1, -1, -1,
            messageDetailPrinter ? messageDetailPrinter->printObjectToString(msg).c_str() : nullptr,
            msg->getPreviousEventNumber());
        entryIndex++;
        removeMessageEntryReference(msg);
    }
}

void EventlogFileManager::componentMethodBegin(cComponent *from, cComponent *to, const char *methodFmt, va_list va)
{
    if (isEventRecordingEnabled && isMethodCallRecordingEnabled) {
        FileLockAcquirer fileLockAcquirer(fileLock, FILE_LOCK_EXCLUSIVE);
        const char *methodText = "";  // for the Enter_Method_Silent case
        if (methodFmt) {
            static char methodTextBuf[MAX_METHODCALL];
            vsnprintf(methodTextBuf, MAX_METHODCALL, methodFmt, va);
            methodTextBuf[MAX_METHODCALL-1] = '\0';
            methodText = methodTextBuf;
        }
        EventLogWriter::recordComponentMethodBeginEntry_sm_tm_m(feventlog, from ? from->getId() : -1, to->getId(), methodText);
        entryIndex++;
    }
}

void EventlogFileManager::componentMethodEnd()
{
    if (isEventRecordingEnabled && isMethodCallRecordingEnabled) {
        FileLockAcquirer fileLockAcquirer(fileLock, FILE_LOCK_EXCLUSIVE);
        // TODO: problem when channel method is called: we'll emit an "End" entry but no "Begin"
        // TODO: same problem when the caller is not a module or is nullptr
        EventLogWriter::recordComponentMethodEndEntry(feventlog);
        entryIndex++;
    }
}

void EventlogFileManager::moduleCreated(cModule *module)
{
    bool recordModuleEvents = envir->getConfig()->getAsBool(module->getFullPath().c_str(), CFGID_MODULE_EVENTLOG_RECORDING);
    module->setRecordEvents(recordModuleEvents);
    if (isEventRecordingEnabled && isModuleRecordingEnabled) {
        FileLockAcquirer fileLockAcquirer(fileLock, FILE_LOCK_EXCLUSIVE);
        bool isCompoundModule = module->hasSubmodules() || !dynamic_cast<cSimpleModule *>(module);
        // FIXME: size() is missing
        EventLogWriter::recordModuleCreatedEntry_id_c_t_pid_n_cm(feventlog, module->getId(), module->getClassName(), module->getNedTypeName(), module->getParentModule() ? module->getParentModule()->getId() : -1, module->getFullName(), isCompoundModule);
        entryIndex++;
        addIndexEventLogEntry(eventNumber, entryIndex);
        moduleToModuleCreatedEntryReferenceMap[module] = EventLogEntryReference(eventNumber, entryIndex);
    }
}

void EventlogFileManager::moduleDeleted(cModule *module)
{
    if (isEventRecordingEnabled && isModuleRecordingEnabled) {
        FileLockAcquirer fileLockAcquirer(fileLock, FILE_LOCK_EXCLUSIVE);
        EventLogWriter::recordModuleDeletedEntry_id(feventlog, module->getId());
        entryIndex++;
        removeIndexEventLogEntry(moduleToModuleCreatedEntryReferenceMap[module]);
        moduleToModuleCreatedEntryReferenceMap.erase(module);
        std::map<cModule *, EventLogEntryReference>::iterator it = moduleToModuleDisplayStringChangedEntryReferenceMap.find(module);
        if (it != moduleToModuleDisplayStringChangedEntryReferenceMap.end())
            removeIndexEventLogEntry((*it).second);
        moduleToModuleDisplayStringChangedEntryReferenceMap.erase(module);
    }
}

void EventlogFileManager::gateCreated(cGate *gate)
{
    if (isEventRecordingEnabled && isModuleRecordingEnabled) {
        FileLockAcquirer fileLockAcquirer(fileLock, FILE_LOCK_EXCLUSIVE);
        EventLogWriter::recordGateCreatedEntry_m_g_n_i_o(feventlog, gate->getOwnerModule()->getId(), gate->getId(), gate->getName(), gate->isVector() ? gate->getIndex() : -1, gate->getType() == cGate::OUTPUT);
        entryIndex++;
        addIndexEventLogEntry(eventNumber, entryIndex);
        gateToGateCreatedEntryReferenceMap[gate] = EventLogEntryReference(eventNumber, entryIndex);
    }
}

void EventlogFileManager::gateDeleted(cGate *gate)
{
    if (isEventRecordingEnabled && isModuleRecordingEnabled) {
        FileLockAcquirer fileLockAcquirer(fileLock, FILE_LOCK_EXCLUSIVE);
        EventLogWriter::recordGateDeletedEntry_m_g(feventlog, gate->getOwnerModule()->getId(), gate->getId());
        entryIndex++;
        removeIndexEventLogEntry(gateToGateCreatedEntryReferenceMap[gate]);
        gateToGateCreatedEntryReferenceMap.erase(gate);
    }
}

void EventlogFileManager::connectionCreated(cGate *srcgate)
{
    if (isEventRecordingEnabled && isModuleRecordingEnabled) {
        FileLockAcquirer fileLockAcquirer(fileLock, FILE_LOCK_EXCLUSIVE);
        cGate *destgate = srcgate->getNextGate();
        // TODO: channel, channel attributes, etc
        EventLogWriter::recordConnectionCreatedEntry_sm_sg_dm_dg(feventlog, srcgate->getOwnerModule()->getId(), srcgate->getId(), destgate->getOwnerModule()->getId(), destgate->getId());
        entryIndex++;
        addIndexEventLogEntry(eventNumber, entryIndex);
        channelToConnectionCreatedEntryReferenceMap[srcgate] = EventLogEntryReference(eventNumber, entryIndex);
        std::map<cGate *, EventLogEntryReference>::iterator it = channelToConnectionDisplayStringChangedEntryReferenceMap.find(srcgate);
        if (it != channelToConnectionDisplayStringChangedEntryReferenceMap.end())
            removeIndexEventLogEntry((*it).second);
        channelToConnectionDisplayStringChangedEntryReferenceMap[srcgate] = EventLogEntryReference(eventNumber, entryIndex);
    }
}

void EventlogFileManager::connectionDeleted(cGate *srcgate)
{
    if (isEventRecordingEnabled && isModuleRecordingEnabled) {
        FileLockAcquirer fileLockAcquirer(fileLock, FILE_LOCK_EXCLUSIVE);
        EventLogWriter::recordConnectionDeletedEntry_sm_sg(feventlog, srcgate->getOwnerModule()->getId(), srcgate->getId());
        entryIndex++;
        removeIndexEventLogEntry(channelToConnectionCreatedEntryReferenceMap[srcgate]);
        channelToConnectionCreatedEntryReferenceMap.erase(srcgate);
    }
}

void EventlogFileManager::displayStringChanged(cComponent *component)
{
    if (isEventRecordingEnabled && isDisplayStringRecordingEnabled) {
        if (dynamic_cast<cModule *>(component)) {
            FileLockAcquirer fileLockAcquirer(fileLock, FILE_LOCK_EXCLUSIVE);
            cModule *module = (cModule *)component;
            EventLogWriter::recordModuleDisplayStringChangedEntry_id_d(feventlog, module->getId(), module->getDisplayString().str());
            entryIndex++;
            addIndexEventLogEntry(eventNumber, entryIndex);
            std::map<cModule *, EventLogEntryReference>::iterator it = moduleToModuleDisplayStringChangedEntryReferenceMap.find(module);
            if (it != moduleToModuleDisplayStringChangedEntryReferenceMap.end())
                removeIndexEventLogEntry((*it).second);
            moduleToModuleDisplayStringChangedEntryReferenceMap[module] = EventLogEntryReference(eventNumber, entryIndex);
        }
        else if (dynamic_cast<cChannel *>(component)) {
            FileLockAcquirer fileLockAcquirer(fileLock, FILE_LOCK_EXCLUSIVE);
            cChannel *channel = (cChannel *)component;
            cGate *gate = channel->getSourceGate();
            EventLogWriter::recordConnectionDisplayStringChangedEntry_sm_sg_d(feventlog, gate->getOwnerModule()->getId(), gate->getId(), channel->getDisplayString().str());
            entryIndex++;
            addIndexEventLogEntry(eventNumber, entryIndex);
            std::map<cGate *, EventLogEntryReference>::iterator it = channelToConnectionDisplayStringChangedEntryReferenceMap.find(gate);
            if (it != channelToConnectionDisplayStringChangedEntryReferenceMap.end())
                removeIndexEventLogEntry((*it).second);
            channelToConnectionDisplayStringChangedEntryReferenceMap[gate] = EventLogEntryReference(eventNumber, entryIndex);
        }
    }
}

void EventlogFileManager::logLine(const char *prefix, const char *text, int length)
{
    if (isEventRecordingEnabled && isTextRecordingEnabled) {
        FileLockAcquirer fileLockAcquirer(fileLock, FILE_LOCK_EXCLUSIVE);
        const char *textEnd = text + length;
        char *line = const_cast<char *>(text);
        while (line < textEnd) {
            char *lineEnd = line;
            while (lineEnd != textEnd && *lineEnd != '\n')
                lineEnd++;
            EventLogWriter::recordLogLine(feventlog, prefix, line, lineEnd - line);
            // TODO: write the escaped new lines into the eventlog file and handle this from the gui
//            if (*lineEnd == '\n')
//                fprintf(feventlog, "\\n");
            fprintf(feventlog, "\n");
            line = lineEnd + 1;
            entryIndex++;
        }
    }
}

void EventlogFileManager::stoppedWithException(bool isError, int resultCode, const char *message)
{
    throw opp_runtime_error("");
}

//========================================================================== index management

void EventlogFileManager::addIndexEventLogEntry(EventLogEntryReference reference)
{
    addIndexEventLogEntry(reference.eventNumber, reference.entryIndex);
}

void EventlogFileManager::addIndexEventLogEntry(eventnumber_t eventNumber, int entryIndex)
{
    addIndexEventLogEntry(eventNumberToSnapshotEventLogEntryRanges, eventNumber, entryIndex);
    addIndexEventLogEntry(eventNumberToAddedIndexEventLogEntryRanges, eventNumber, entryIndex);
}

bool EventlogFileManager::addIndexEventLogEntry(std::map<eventnumber_t, std::vector<EventLogEntryRange> >& eventNumberToIndexEventLogEntryRanges, eventnumber_t eventNumber, int entryIndex)
{
    ASSERT(eventNumber != -1);
    ASSERT(entryIndex != -1);
    std::map<eventnumber_t, std::vector<EventLogEntryRange> >::iterator it = eventNumberToIndexEventLogEntryRanges.find(eventNumber);
    if (it != eventNumberToIndexEventLogEntryRanges.end()) {
        std::vector<EventLogEntryRange> &ranges = it->second;
        EventLogEntryRange &back = ranges.back();
        if (back.eventNumber == eventNumber && back.endEntryIndex == entryIndex - 1)
            back.endEntryIndex++;
        else
            ranges.push_back(EventLogEntryRange(eventNumber, entryIndex, entryIndex));
    }
    else {
        std::vector<EventLogEntryRange> ranges;
        ranges.push_back(EventLogEntryRange(eventNumber, entryIndex, entryIndex));
        eventNumberToIndexEventLogEntryRanges[eventNumber] = ranges;
    }
    return true;
}

void EventlogFileManager::removeIndexEventLogEntry(EventLogEntryReference reference)
{
    removeIndexEventLogEntry(reference.eventNumber, reference.entryIndex);
}

void EventlogFileManager::removeIndexEventLogEntry(eventnumber_t eventNumber, int entryIndex)
{
    removeIndexEventLogEntry(eventNumberToSnapshotEventLogEntryRanges, eventNumber, entryIndex);
    // first try removing from the list of added entries
    // if not found then add to the list of removed entries
    if (!removeIndexEventLogEntry(eventNumberToAddedIndexEventLogEntryRanges, eventNumber, entryIndex))
        addIndexEventLogEntry(eventNumberToRemovedIndexEventLogEntryRanges, eventNumber, entryIndex);
}

bool EventlogFileManager::removeIndexEventLogEntry(std::map<eventnumber_t, std::vector<EventLogEntryRange> >& eventNumberToIndexEventLogEntryRanges, eventnumber_t eventNumber, int entryIndex)
{
    ASSERT(eventNumber != -1);
    ASSERT(entryIndex != -1);
    std::map<eventnumber_t, std::vector<EventLogEntryRange> >::iterator it = eventNumberToIndexEventLogEntryRanges.find(eventNumber);
    if (it != eventNumberToIndexEventLogEntryRanges.end()) {
        std::vector<EventLogEntryRange> &ranges = it->second;
        for (std::vector<EventLogEntryRange>::iterator jt = ranges.begin(); jt != ranges.end(); jt++) {
            EventLogEntryRange eventLogEntryRange = *jt;
            int beginEntryIndex = eventLogEntryRange.beginEntryIndex;
            int endEntryIndex = eventLogEntryRange.endEntryIndex;
            if (eventLogEntryRange.eventNumber == eventNumber && beginEntryIndex <= entryIndex && entryIndex <= endEntryIndex) {
                ranges.erase(jt);
                if (eventLogEntryRange.beginEntryIndex != eventLogEntryRange.endEntryIndex) {
                    if (beginEntryIndex != entryIndex)
                        ranges.push_back(EventLogEntryRange(eventNumber, beginEntryIndex, entryIndex - 1));
                    if (endEntryIndex != entryIndex)
                        ranges.push_back(EventLogEntryRange(eventNumber, entryIndex + 1, endEntryIndex));
                }
                if (ranges.size() == 0)
                    eventNumberToIndexEventLogEntryRanges.erase(it);
                return true;
            }
        }
    }
    return false;
}

void EventlogFileManager::addMessageEntryReference(cMessage *msg)
{
    addIndexEventLogEntry(eventNumber, entryIndex);
    messageToEntryReferenceMap[msg] = EventLogEntryReference(eventNumber, entryIndex);
}

void EventlogFileManager::removeMessageEntryReference(cMessage *msg)
{
    std::map<cMessage *, EventLogEntryReference>::iterator it = messageToEntryReferenceMap.find(msg);
    if (it != messageToEntryReferenceMap.end()) {
        removeIndexEventLogEntry(it->second);
        messageToEntryReferenceMap.erase(it);
    }
}

void EventlogFileManager::recordSimulationBegin()
{
    FileLockAcquirer fileLockAcquirer(fileLock, FILE_LOCK_EXCLUSIVE);
    beginningFileOffset = 0;
    const char *runId = envir->getConfigEx()->getVariable(CFGVAR_RUNID);
    EventLogWriter::recordSimulationBeginEntry_ov_ev_rid(feventlog, OMNETPP_VERSION, EVENTLOG_VERSION, runId);
    eventNumber = -1;
    entryIndex = 0;
    lastChunk = BEGIN;
    fflush(feventlog);
}

void EventlogFileManager::recordSimulationEnd(bool isError, int resultCode, const char *message)
{
    FileLockAcquirer fileLockAcquirer(fileLock, FILE_LOCK_EXCLUSIVE);
    fprintf(feventlog, "\n");
    EventLogWriter::recordSimulationEndEntry_e_c_m(feventlog, isError, resultCode, message);
    eventNumber = -1;
    entryIndex = 0;
    lastChunk = END;
    fflush(feventlog);
}

void EventlogFileManager::recordInitialize()
{
    FileLockAcquirer fileLockAcquirer(fileLock, FILE_LOCK_EXCLUSIVE);
    fprintf(feventlog, "\n");
    // we can't use cSimulation::getActiveSimulation()->getEventNumber() and cSimulation::getActiveSimulation()->getSimTime(), because when we start a new run
    // these numbers are still set from the previous run (i.e. not zero)
    EventLogWriter::recordEventEntry_e_t_m_ce_msg(feventlog, 0, 0, 1, -1, -1);
    eventNumber = 0;
    entryIndex = 0;
    fflush(feventlog);
}

void EventlogFileManager::recordSnapshot()
{
    // TODO: shouldn't we clear the index here?
    FileLockAcquirer fileLockAcquirer(fileLock, FILE_LOCK_EXCLUSIVE);
    fprintf(feventlog, "\n");
    file_offset_t snapshotFileOffset = toVirtualFileOffset(opp_ftell(feventlog));
    EventLogWriter::recordSnapshotEntry_f_e_t(feventlog, toVirtualFileOffset(opp_ftell(feventlog)), eventNumber, simulationTime);
    entryIndex = 0;
    previousSnapshotFileOffset = snapshotFileOffset;
    cModule *systemModule = cSimulation::getActiveSimulation()->getSystemModule();
    if (systemModule && isModuleRecordingEnabled) {
        recordModules(systemModule);
        recordConnections(systemModule);
    }
    if (isMessageRecordingEnabled) {
        recordMessages(systemModule);
        recordMessages(cSimulation::getActiveSimulation()->getFES());
    }
    for (std::map<eventnumber_t, std::vector<EventLogEntryRange> >::iterator it = eventNumberToSnapshotEventLogEntryRanges.begin(); it != eventNumberToSnapshotEventLogEntryRanges.end(); it++) {
        std::vector<EventLogEntryRange> &ranges = it->second;
        for (std::vector<EventLogEntryRange>::iterator jt = ranges.begin(); jt != ranges.end(); jt++)
            EventLogWriter::recordReferenceFoundEntry_e_b_e(feventlog, jt->eventNumber, jt->beginEntryIndex, jt->endEntryIndex);
    }
    lastChunk = SNAPSHOT;
    fflush(feventlog);
}

void EventlogFileManager::recordModules(cModule *module)
{
    bool recordModuleEvents = envir->getConfig()->getAsBool(module->getFullPath().c_str(), CFGID_MODULE_EVENTLOG_RECORDING);
    module->setRecordEvents(recordModuleEvents);
    FileLockAcquirer fileLockAcquirer(fileLock, FILE_LOCK_EXCLUSIVE);
    cModule *parentModule = module->getParentModule();
    bool isCompoundModule = module->hasSubmodules() || !dynamic_cast<cSimpleModule *>(module);
    // FIXME: size() is missing
    std::map<cModule *, EventLogEntryReference>::iterator mit = moduleToModuleCreatedEntryReferenceMap.find(module);
    EventLogWriter::recordModuleFoundEntry_id_c_t_pid_n_cm_e_ei(feventlog, module->getId(), module->getClassName(), module->getNedTypeName(), parentModule ? parentModule->getId() : -1, module->getFullName(), isCompoundModule, mit->second.eventNumber, mit->second.entryIndex);
    entryIndex++;
    for (cModule::GateIterator it(module); !it.end(); it++) {
        cGate *gate = *it;
        std::map<cGate *, EventLogEntryReference>::iterator git = gateToGateCreatedEntryReferenceMap.find(gate);
        EventLogWriter::recordGateFoundEntry_m_g_n_i_o_e_ei(feventlog, gate->getOwnerModule()->getId(), gate->getId(), gate->getName(), gate->isVector() ? gate->getIndex() : -1, gate->getType() == cGate::OUTPUT, git->second.eventNumber, git->second.entryIndex);
        entryIndex++;
    }
    std::map<cModule *, EventLogEntryReference>::iterator dit = moduleToModuleDisplayStringChangedEntryReferenceMap.find(module);
    EventLogWriter::recordModuleDisplayStringFoundEntry_id_d_e_ei(feventlog, module->getId(), module->getDisplayString().str(), dit->second.eventNumber, dit->second.entryIndex);
    entryIndex++;
    for (cModule::SubmoduleIterator it(module); !it.end(); it++)
        recordModules(*it);
}

void EventlogFileManager::recordConnections(cModule *module)
{
    FileLockAcquirer fileLockAcquirer(fileLock, FILE_LOCK_EXCLUSIVE);
    for (cModule::GateIterator it(module); !it.end(); it++) {
        cGate *srcgate = *it;
        cChannel *channel = srcgate->getChannel();
        if (srcgate->getNextGate()) {
            cGate *destgate = srcgate->getNextGate();
            std::map<cGate *, EventLogEntryReference>::iterator cit = channelToConnectionCreatedEntryReferenceMap.find(srcgate);
            EventLogWriter::recordConnectionFoundEntry_sm_sg_dm_dg_e_ei(feventlog, srcgate->getOwnerModule()->getId(), srcgate->getId(), destgate->getOwnerModule()->getId(), destgate->getId(), cit->second.eventNumber, cit->second.entryIndex);
            entryIndex++;
        }
        if (channel) {
            std::map<cGate *, EventLogEntryReference>::iterator dit = channelToConnectionDisplayStringChangedEntryReferenceMap.find(srcgate);
            EventLogWriter::recordConnectionDisplayStringFoundEntry_sm_sg_d_e_ei(feventlog, srcgate->getOwnerModule()->getId(), srcgate->getId(), channel->getDisplayString().str(), dit->second.eventNumber, dit->second.entryIndex);
            entryIndex++;
        }
    }
    for (cModule::SubmoduleIterator it(module); !it.end(); it++)
        recordConnections(*it);
}

void EventlogFileManager::recordMessages(cObject *owner)
{
    cFutureEventSet *messageHeap = dynamic_cast<cFutureEventSet *>(owner);
    if (messageHeap) {
        int size = messageHeap->getLength();
        for (int i = 0; i < size; i++) {
            cMessage * message = dynamic_cast<cMessage *>(messageHeap->get(i));
            if (message)
                recordMessage(message);
        }
    }
    cSoftOwner *softOwner = dynamic_cast<cSoftOwner *>(owner);
    if (softOwner) {
        int size = softOwner->getNumOwnedObjects();
        for (int i = 0; i < size; i++)
            recordMessages(softOwner->getOwnedObject(i));
    }
    cMessage *msg = dynamic_cast<cMessage *>(owner);
    if (msg)
        recordMessage(msg);
}

void EventlogFileManager::recordMessage(cMessage *msg)
{
    FileLockAcquirer fileLockAcquirer(fileLock, FILE_LOCK_EXCLUSIVE);
    cModule *ownerModule = dynamic_cast<cModule *>(msg->getOwner());
    bool isScheduled = msg->isScheduled();
    bool isPacket = msg->isPacket();
    cPacket *pkt = isPacket ? (cPacket *)msg : nullptr;
    EventLogWriter::recordMessageFoundEntry_id_tid_eid_etid_c_n_k_p_l_er_m_sm_sg_st_am_ag_at_d_pe(feventlog,
        msg->getId(), msg->getTreeId(), isPacket ? pkt->getEncapsulationId() : msg->getId(), isPacket ? pkt->getEncapsulationTreeId() : msg->getTreeId(),
        msg->getClassName(), msg->getFullName(),
        msg->getKind(), msg->getSchedulingPriority(), isPacket ? pkt->getBitLength() : 0, isPacket ? pkt->hasBitError() : false,
        ownerModule ? ownerModule->getId() : -1,
        isScheduled ? msg->getSenderModuleId() : -1, isScheduled ? msg->getSenderGateId() : -1, isScheduled ? msg->getSendingTime() : -1,
        isScheduled ? msg->getArrivalModuleId() : -1, isScheduled ? msg->getArrivalGateId() : -1, isScheduled ? msg->getArrivalTime() : -1,
        messageDetailPrinter ? messageDetailPrinter->printObjectToString(msg).c_str() : nullptr,
        msg->getPreviousEventNumber());
    entryIndex++;
}

void EventlogFileManager::recordIndex()
{
    FileLockAcquirer fileLockAcquirer(fileLock, FILE_LOCK_EXCLUSIVE);
    fprintf(feventlog, "\n");
    file_offset_t indexFileOffset = toVirtualFileOffset(opp_ftell(feventlog));
    EventLogWriter::recordIndexEntry_f_i_s_e_t(feventlog, indexFileOffset, previousIndexFileOffset, previousSnapshotFileOffset, eventNumber, simulationTime);
    entryIndex = 0;
    for (std::map<eventnumber_t, std::vector<EventLogEntryRange> >::iterator it = eventNumberToRemovedIndexEventLogEntryRanges.begin(); it != eventNumberToRemovedIndexEventLogEntryRanges.end(); it++) {
        std::vector<EventLogEntryRange> &ranges = it->second;
        for (std::vector<EventLogEntryRange>::iterator jt = ranges.begin(); jt != ranges.end(); jt++) {
            EventLogWriter::recordReferenceRemovedEntry_e_b_e(feventlog, jt->eventNumber, jt->beginEntryIndex, jt->endEntryIndex);
            entryIndex++;
        }
    }
    for (std::map<eventnumber_t, std::vector<EventLogEntryRange> >::iterator it = eventNumberToAddedIndexEventLogEntryRanges.begin(); it != eventNumberToAddedIndexEventLogEntryRanges.end(); it++) {
        std::vector<EventLogEntryRange> &ranges = it->second;
        for (std::vector<EventLogEntryRange>::iterator jt = ranges.begin(); jt != ranges.end(); jt++) {
            EventLogWriter::recordReferenceAddedEntry_e_b_e(feventlog, jt->eventNumber, jt->beginEntryIndex, jt->endEntryIndex);
            entryIndex++;
        }
    }
    eventNumberToAddedIndexEventLogEntryRanges.clear();
    eventNumberToRemovedIndexEventLogEntryRanges.clear();
    previousIndexFileOffset = indexFileOffset;
    lastChunk = INDEX;
    fflush(feventlog);
}

}  // namespace envir
}  // namespace omnetpp

