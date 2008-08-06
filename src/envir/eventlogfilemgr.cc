//==========================================================================
//  EVENTLOGFILEMGR.CC - part of
//                     OMNeT++/OMNEST
//            Discrete System Simulation in C++
//
//  Author: Andras Varga
//
//==========================================================================

/*--------------------------------------------------------------*
  Copyright (C) 1992-2008 Andras Varga
  Copyright (C) 2006-2008 OpenSim Ltd.

  This file is distributed WITHOUT ANY WARRANTY. See the file
  `license' for details on this and other legal matters.
*--------------------------------------------------------------*/

#include "opp_ctype.h"
#include "commonutil.h"  //vsnprintf
#include "eventlogfilemgr.h"
#include "eventlogwriter.h"
#include "cconfigkey.h"
#include "fileutil.h"
#include "cconfiguration.h"
#include "envirbase.h"
#include "cmodule.h"
#include "cmessage.h"
#include "cgate.h"
#include "cchannel.h"
#include "csimplemodule.h"
#include "ccompoundmodule.h"
#include "cdisplaystring.h"

USING_NAMESPACE


Register_PerRunConfigEntry(CFGID_EVENTLOG_FILE, "eventlog-file", CFG_FILENAME, "${resultdir}/${configname}-${runnumber}.log", "Name of the event log file to generate.");
Register_PerRunConfigEntry(CFGID_EVENTLOG_MESSAGE_DETAIL_PATTERN, "eventlog-message-detail-pattern", CFG_CUSTOM, NULL,
        "A list of patterns separated by '|' character which will be used to write "
        "message detail information into the event log for each message sent during "
        "the simulation. The message detail will be presented in the sequence chart "
        "tool. Each pattern starts with an object pattern optionally followed by ':' "
        "character and a comma separated list of field patterns. In both "
        "patterns and/or/not/* and various field match expressions can be used. "
        "The object pattern matches to class name, the field pattern matches to field name by default.\n"
        "  EVENTLOG-MESSAGE-DETAIL-PATTERN := ( DETAIL-PATTERN '|' )* DETAIL_PATTERN\n"
        "  DETAIL-PATTERN := OBJECT-PATTERN [ ':' FIELD-PATTERNS ]\n"
        "  OBJECT-PATTERN := MATCH-EXPRESSION\n"
        "  FIELD-PATTERNS := ( FIELD-PATTERN ',' )* FIELD_PATTERN\n"
        "  FIELD-PATTERN := MATCH-EXPRESSION\n"
        "Examples (enter them without quotes):\n"
        "  \"*\": captures all fields of all messages\n"
        "  \"*Frame:*Address,*Id\": captures all fields named somethingAddress and somethingId from messages of any class named somethingFrame\n"
        "  \"MyMessage:declaredOn(MyMessage)\": captures instances of MyMessage recording the fields declared on the MyMessage class\n"
        "  \"*:(not declaredOn(cMessage) and not declaredOn(cNamedObject) and not declaredOn(cObject))\": records user-defined fields from all messages");
Register_PerRunConfigEntry(CFGID_EVENTLOG_RECORDING_INTERVALS, "eventlog-recording-intervals", CFG_CUSTOM, NULL, "Simulation time interval(s) when events should be recorded. Syntax: [<from>]..[<to>],... That is, both start and end of an interval are optional, and intervals are separated by comma. Example: ..10.2, 22.2..100, 233.3..");
Register_PerObjectConfigEntry(CFGID_MODULE_EVENTLOG_RECORDING, "module-eventlog-recording", CFG_BOOL, "true", "Enables recording events on a per module basis. This is meaningful for simple modules only. \nExample:\n **.router[10..20].**.module-eventlog-recording = true\n **.module-eventlog-recording = false");


EventlogFileManager::EventlogFileManager()
{
    feventlog = NULL;
    objectPrinter = NULL;
    recordingIntervals = NULL;
    isEventLogRecordingEnabled = true;
    isIntervalEventLogRecordingEnabled = true;
    isModuleEventLogRecordingEnabled = true;
}

EventlogFileManager::~EventlogFileManager()
{
    delete objectPrinter;
    delete recordingIntervals;
}

void EventlogFileManager::setup()
{
    // setup event log object printer
    delete objectPrinter;
    objectPrinter = NULL;

    const char *eventLogMessageDetailPattern = ev.getConfig()->getAsCustom(CFGID_EVENTLOG_MESSAGE_DETAIL_PATTERN);

    if (eventLogMessageDetailPattern) {
        objectPrinter = new ObjectPrinter(eventLogMessageDetailPattern, 3);
    }

    // setup eventlog recording intervals
    const char *text = ev.getConfig()->getAsCustom(CFGID_EVENTLOG_RECORDING_INTERVALS);
    if (text) {
        recordingIntervals = new Intervals();
        recordingIntervals->parse(text);
    }

    // setup file
    filename = ev.getConfig()->getAsFilename(CFGID_EVENTLOG_FILE).c_str();
    dynamic_cast<EnvirBase *>(&ev)->processFileName(filename);
    ::printf("Recording event log to file `%s'...\n", filename.c_str());
    mkPath(directoryOf(filename.c_str()).c_str());
    FILE *out = fopen(filename.c_str(), "w");
    if (!out)
        throw cRuntimeError("Cannot open eventlog file `%s' for write", filename.c_str());
    feventlog = out;
}

void EventlogFileManager::startRun(const char *runId)
{
    if (isEventLogRecordingEnabled)
    {
        EventLogWriter::recordEventEntry_e_t_m_msg(feventlog, 0, 0, simulation.getSystemModule()->getId(), -1);
        EventLogWriter::recordSimulationBeginEntry_v_rid(feventlog, OMNETPP_VERSION, runId);
        fflush(feventlog);
    }
}

void EventlogFileManager::endRun()
{
    if (isEventLogRecordingEnabled)
    {
        EventLogWriter::recordSimulationEndEntry(feventlog);
        fclose(feventlog);
        feventlog = NULL;
    }
}

void EventlogFileManager::flush()
{
    if (isEventLogRecordingEnabled)
    {
        fflush(feventlog);
    }
}

void EventlogFileManager::simulationEvent(cMessage *msg)
{
    if (isEventLogRecordingEnabled)
    {
        cModule *mod = simulation.getContextModule();

        isModuleEventLogRecordingEnabled = simulation.getContextModule()->isRecordEvents();
        isIntervalEventLogRecordingEnabled = !recordingIntervals || recordingIntervals->contains(simulation.getSimTime());
        isEventLogRecordingEnabled = isModuleEventLogRecordingEnabled && isIntervalEventLogRecordingEnabled;

        EventLogWriter::recordEventEntry_e_t_m_ce_msg(feventlog,
            simulation.getEventNumber(), simulation.getSimTime(), mod->getId(),
            msg->getPreviousEventNumber(), msg->getId());
    }
}

void EventlogFileManager::bubble(cComponent *component, const char *text)
{
    if (isEventLogRecordingEnabled)
    {
        if (dynamic_cast<cModule *>(component))
        {
            cModule *mod = (cModule *)component;
            EventLogWriter::recordBubbleEntry_id_txt(feventlog, mod->getId(), text);
        }
        else if (dynamic_cast<cChannel *>(component))
        {
            //TODO
        }
    }
}

void EventlogFileManager::beginSend(cMessage *msg)
{
    if (isEventLogRecordingEnabled)
    {
        //TODO record message display string as well?
        if (msg->isPacket()) {
            cPacket *pkt = (cPacket *)msg;
            EventLogWriter::recordBeginSendEntry_id_tid_eid_etid_c_n_pe_k_p_l_er_d(feventlog,
                pkt->getId(), pkt->getTreeId(), pkt->getEncapsulationId(), pkt->getEncapsulationTreeId(),
                pkt->getClassName(), pkt->getFullName(), pkt->getPreviousEventNumber(),
                pkt->getKind(), pkt->getPriority(), pkt->getBitLength(), pkt->hasBitError(),
                objectPrinter ? objectPrinter->printObjectToString(pkt).c_str() : NULL);
        }
        else {
            EventLogWriter::recordBeginSendEntry_id_tid_eid_etid_c_n_pe_k_p_l_er_d(feventlog,
                msg->getId(), msg->getTreeId(), msg->getId(), msg->getTreeId(),
                msg->getClassName(), msg->getFullName(), msg->getPreviousEventNumber(),
                msg->getKind(), msg->getPriority(), 0, false,
                objectPrinter ? objectPrinter->printObjectToString(msg).c_str() : NULL);
        }
    }
}

void EventlogFileManager::messageScheduled(cMessage *msg)
{
    if (isEventLogRecordingEnabled)
    {
        EventlogFileManager::beginSend(msg);
        EventlogFileManager::endSend(msg);
    }
}

void EventlogFileManager::messageCancelled(cMessage *msg)
{
    if (isEventLogRecordingEnabled)
    {
        EventLogWriter::recordCancelEventEntry_id_pe(feventlog, msg->getId(), msg->getPreviousEventNumber());
    }
}

void EventlogFileManager::messageSendDirect(cMessage *msg, cGate *toGate, simtime_t propagationDelay, simtime_t transmissionDelay)
{
    if (isEventLogRecordingEnabled)
    {
        EventLogWriter::recordSendDirectEntry_sm_dm_dg_pd_td(feventlog,
            msg->getSenderModuleId(), toGate->getOwnerModule()->getId(), toGate->getId(),
            propagationDelay, transmissionDelay);
    }
}

void EventlogFileManager::messageSendHop(cMessage *msg, cGate *srcGate)
{
    if (isEventLogRecordingEnabled)
    {
        EventLogWriter::recordSendHopEntry_sm_sg(feventlog,
            srcGate->getOwnerModule()->getId(), srcGate->getId());
    }
}

void EventlogFileManager::messageSendHop(cMessage *msg, cGate *srcGate, simtime_t propagationDelay, simtime_t transmissionDelay)
{
    if (isEventLogRecordingEnabled)
    {
        EventLogWriter::recordSendHopEntry_sm_sg_pd_td(feventlog,
            srcGate->getOwnerModule()->getId(), srcGate->getId(), propagationDelay, transmissionDelay);
    }
}

void EventlogFileManager::endSend(cMessage *msg)
{
    if (isEventLogRecordingEnabled)
    {
        bool isStart = msg->isPacket() ? ((cPacket *)msg)->isReceptionStart() : false;
        EventLogWriter::recordEndSendEntry_t_is(feventlog, msg->getArrivalTime(), isStart);
    }
}

void EventlogFileManager::messageDeleted(cMessage *msg)
{
    if (isEventLogRecordingEnabled)
    {
        EventLogWriter::recordDeleteMessageEntry_id_pe(feventlog, msg->getId(), msg->getPreviousEventNumber());
    }
}

void EventlogFileManager::componentMethodBegin(cComponent *from, cComponent *to, const char *methodFmt, va_list va)
{
    if (isEventLogRecordingEnabled)
    {
        if (from->isModule() && to->isModule())
        {
            const char *methodText = "";  // for the Enter_Method_Silent case
            if (methodFmt)
            {
                static char methodTextBuf[MAX_METHODCALL];
                vsnprintf(methodTextBuf, MAX_METHODCALL, methodFmt, va);
                methodTextBuf[MAX_METHODCALL-1] = '\0';
                methodText = methodTextBuf;
            }
            EventLogWriter::recordModuleMethodBeginEntry_sm_tm_m(feventlog,
                ((cModule *)from)->getId(), ((cModule *)to)->getId(), methodText);
        }
    }
}

void EventlogFileManager::componentMethodEnd()
{
    if (isEventLogRecordingEnabled)
    {
        //XXX problem when channel method is called: we'll emit an "End" entry but no "Begin"
        EventLogWriter::recordModuleMethodEndEntry(feventlog);
    }
}

void EventlogFileManager::moduleCreated(cModule *newmodule)
{
    if (isEventLogRecordingEnabled)
    {
        cModule *m = newmodule;

        bool recordModuleEvents = ev.getConfig()->getAsBool(m->getFullPath().c_str(), CFGID_MODULE_EVENTLOG_RECORDING);
        m->setRecordEvents(recordModuleEvents);

        bool isCompoundModule = dynamic_cast<cCompoundModule *>(m);

        EventLogWriter::recordModuleCreatedEntry_id_c_t_pid_n_cm(feventlog,
            m->getId(), m->getClassName(), m->getNedTypeName(), m->getParentModule() ? m->getParentModule()->getId() : -1, m->getFullName(), isCompoundModule); //FIXME size() is missing
    }
}

void EventlogFileManager::moduleDeleted(cModule *module)
{
    if (isEventLogRecordingEnabled)
    {
        EventLogWriter::recordModuleDeletedEntry_id(feventlog, module->getId());
    }
}

void EventlogFileManager::moduleReparented(cModule *module, cModule *oldparent)
{
    if (isEventLogRecordingEnabled)
    {
        EventLogWriter::recordModuleReparentedEntry_id_p(feventlog, module->getId(), module->getParentModule()->getId());
    }
}

void EventlogFileManager::gateCreated(cGate *newgate)
{
    if (isEventLogRecordingEnabled)
    {
        EventLogWriter::recordGateCreatedEntry_m_g_n_i_o(feventlog,
            newgate->getOwnerModule()->getId(), newgate->getId(), newgate->getName(),
            newgate->isVector() ? newgate->getIndex() : -1, newgate->getType() == cGate::OUTPUT);
    }
}

void EventlogFileManager::gateDeleted(cGate *gate)
{
    if (isEventLogRecordingEnabled)
    {
        EventLogWriter::recordGateDeletedEntry_m_g(feventlog, gate->getOwnerModule()->getId(), gate->getId());
    }
}

void EventlogFileManager::connectionCreated(cGate *srcgate)
{
    if (isEventLogRecordingEnabled)
    {
        cGate *destgate = srcgate->getToGate();
        EventLogWriter::recordConnectionCreatedEntry_sm_sg_dm_dg(feventlog,
            srcgate->getOwnerModule()->getId(), srcgate->getId(), destgate->getOwnerModule()->getId(), destgate->getId());  //XXX channel, channel attributes, etc
    }
}

void EventlogFileManager::connectionDeleted(cGate *srcgate)
{
    if (isEventLogRecordingEnabled)
    {
        EventLogWriter::recordConnectionDeletedEntry_sm_sg(feventlog,
            srcgate->getOwnerModule()->getId(), srcgate->getId());
    }
}

void EventlogFileManager::displayStringChanged(cComponent *component)
{
    if (isEventLogRecordingEnabled)
    {
        if (dynamic_cast<cModule *>(component))
        {
            cModule *module = (cModule *)component;
            EventLogWriter::recordModuleDisplayStringChangedEntry_id_d(feventlog,
                module->getId(), module->getDisplayString().str());
        }
        else if (dynamic_cast<cChannel *>(component))
        {
            cChannel *channel = (cChannel *)component;
            cGate *gate = channel->getFromGate();
            EventLogWriter::recordConnectionDisplayStringChangedEntry_sm_sg_d(feventlog,
                gate->getOwnerModule()->getId(), gate->getId(), channel->getDisplayString().str());
        }
    }
}

void EventlogFileManager::sputn(const char *s, int n)
{
    if (isEventLogRecordingEnabled)
    {
        EventLogWriter::recordLogLine(feventlog, s, n);
    }
}

