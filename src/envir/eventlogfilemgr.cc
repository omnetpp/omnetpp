//==========================================================================
//  EVENTLOGFILEMGR.CC - part of
//                     OMNeT++/OMNEST
//            Discrete System Simulation in C++
//
//==========================================================================

/*--------------------------------------------------------------*
  Copyright (C) 1992-2005 Andras Varga

  This file is distributed WITHOUT ANY WARRANTY. See the file
  `license' for details on this and other legal matters.
*--------------------------------------------------------------*/

#include "opp_ctype.h"
#include "eventlogfilemgr.h"
#include "eventlogwriter.h"
#include "stringtokenizer.h"
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
        "patterns and/or/not/* and various field matcher expressions can be used. "
        "The object pattern matches to class name, the field pattern matches to field name by default.\n"
        "  EVENTLOG-MESSAGE-DETAIL-PATTERN := ( DETAIL-PATTERN '|' )* DETAIL_PATTERN\n"
        "  DETAIL-PATTERN := OBJECT-PATTERN [ ':' FIELD-PATTERNS ]\n"
        "  OBJECT-PATTERN := MATCHER-EXPRESSION\n"
        "  FIELD-PATTERNS := ( FIELD-PATTERN ',' )* FIELD_PATTERN\n"
        "  FIELD-PATTERN := MATCHER-EXPRESSION\n"
        "Examples (enter them without quotes):\n"
        "  \"*\": captures all fields of all messages\n"
        "  \"*Frame:*Address,*Id\": captures all fields named somethingAddress and somethingId from messages of any class named somethingFrame\n"
        "  \"MyMessage:declaredOn(MyMessage)\": captures instances of MyMessage recording the fields declared on the MyMessage class\n"
        "  \"*:(not declaredOn(cMessage) and not declaredOn(cNamedObject) and not declaredOn(cObject))\": records user-defined fields from all messages");
Register_PerRunConfigEntry(CFGID_EVENTLOG_RECORDING_INTERVALS, "eventlog-recording-intervals", CFG_CUSTOM, NULL, "Interval(s) when events should be recorded. Syntax: [<from>]..[<to>],... That is, both start and end of an interval are optional, and intervals are separated by comma. Example: ..100, 200..400, 900..");
Register_PerObjectConfigEntry(CFGID_RECORD_MODULE_EVENTS, "record-module-events", CFG_BOOL, "true", "Enables recording events on a per module basis. This is meaningful for simple modules only. \nExample:\n **.router[10..20].**.record-module-events = true\n **.record-module-events = false");


//TODO: autoflush for feventlog (after each event? after each line?)
//TODO: bring flags into this class from EventLogWriter? isModuleEventLogRecordingEnabled, isIntervalEventLogRecordingEnabled, isEventLogRecordingEnabled;

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

    const char *eventLogMessageDetailPattern = ev.config()->getAsCustom(CFGID_EVENTLOG_MESSAGE_DETAIL_PATTERN);

    if (eventLogMessageDetailPattern) {
        std::vector<MatchExpression> objectMatchExpressions;
        std::vector<std::vector<MatchExpression> > fieldNameMatchExpressionsList;

        StringTokenizer tokenizer(eventLogMessageDetailPattern, "|"); // TODO: use ; when it does not mean comment anymore
        std::vector<std::string> patterns = tokenizer.asVector();

        for (int i = 0; i < (int)patterns.size(); i++) {
            char *objectPattern = (char *)patterns[i].c_str();
            char *fieldNamePattern = strchr(objectPattern, ':');

            if (fieldNamePattern) {
                *fieldNamePattern = '\0';
                StringTokenizer fieldNameTokenizer(fieldNamePattern + 1, ",");
                std::vector<std::string> fieldNamePatterns = fieldNameTokenizer.asVector();
                std::vector<MatchExpression> fieldNameMatchExpressions;

                for (int j = 0; j < (int)fieldNamePatterns.size(); j++)
                    fieldNameMatchExpressions.push_back(MatchExpression(fieldNamePatterns[j].c_str(), false, true, true));

                fieldNameMatchExpressionsList.push_back(fieldNameMatchExpressions);
            }
            else {
                std::vector<MatchExpression> fieldNameMatchExpressions;
                fieldNameMatchExpressions.push_back(MatchExpression("*", false, true, true));
                fieldNameMatchExpressionsList.push_back(fieldNameMatchExpressions);
            }

            objectMatchExpressions.push_back(MatchExpression(objectPattern, false, true, true));
        }

        objectPrinter = new ObjectPrinter(objectMatchExpressions, fieldNameMatchExpressionsList, 3);
    }

    // setup eventlog recording intervals
    const char *text = ev.config()->getAsCustom(CFGID_EVENTLOG_RECORDING_INTERVALS);
    if (text) {
        recordingIntervals = new Intervals();
        recordingIntervals->parse(text);
    }

    // setup file
    filename = ev.config()->getAsFilename(CFGID_EVENTLOG_FILE).c_str();
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
        cModule *mod = simulation.contextModule();

        isModuleEventLogRecordingEnabled = simulation.contextModule()->isRecordEvents();
        isIntervalEventLogRecordingEnabled = !recordingIntervals || recordingIntervals->contains(simulation.simTime());
        isEventLogRecordingEnabled = isModuleEventLogRecordingEnabled && isIntervalEventLogRecordingEnabled;

        EventLogWriter::recordEventEntry_e_t_m_ce_msg(feventlog,
            simulation.eventNumber(), simulation.simTime(), mod->id(),
            msg->previousEventNumber(), msg->id());
    }
}

void EventlogFileManager::bubble(cComponent *component, const char *text)
{
    if (isEventLogRecordingEnabled)
    {
        if (dynamic_cast<cModule *>(component))
        {
            cModule *mod = (cModule *)component;
            EventLogWriter::recordBubbleEntry_id_txt(feventlog, mod->id(), text);
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
        EventLogWriter::recordBeginSendEntry_id_tid_eid_etid_c_n_pe_k_p_l_er_d(feventlog,
            msg->id(), msg->treeId(), msg->encapsulationId(), msg->encapsulationTreeId(),
            msg->className(), msg->fullName(), msg->previousEventNumber(),
            msg->kind(), msg->priority(), msg->length(), msg->hasBitError(),
            objectPrinter ? objectPrinter->printObjectToString(msg).c_str() : NULL);
            //XXX record message display string as well, and many other fields...?
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
        EventLogWriter::recordCancelEventEntry_id_pe(feventlog, msg->id(), msg->previousEventNumber());
    }
}

void EventlogFileManager::messageSendDirect(cMessage *msg, cGate *toGate, simtime_t propagationDelay, simtime_t transmissionDelay)
{
    if (isEventLogRecordingEnabled)
    {
        EventLogWriter::recordSendDirectEntry_sm_dm_dg_pd_td(feventlog,
            msg->senderModuleId(), toGate->ownerModule()->id(), toGate->id(),
            propagationDelay, transmissionDelay);
    }
}

void EventlogFileManager::messageSendHop(cMessage *msg, cGate *srcGate)
{
    if (isEventLogRecordingEnabled)
    {
        EventLogWriter::recordSendHopEntry_sm_sg(feventlog,
            srcGate->ownerModule()->id(), srcGate->id());
    }
}

void EventlogFileManager::messageSendHop(cMessage *msg, cGate *srcGate, simtime_t propagationDelay, simtime_t transmissionDelay)
{
    if (isEventLogRecordingEnabled)
    {
        EventLogWriter::recordSendHopEntry_sm_sg_pd_td(feventlog,
            srcGate->ownerModule()->id(), srcGate->id(), transmissionDelay, propagationDelay);
    }
}

void EventlogFileManager::endSend(cMessage *msg)
{
    if (isEventLogRecordingEnabled)
    {
        EventLogWriter::recordEndSendEntry_t(feventlog, msg->arrivalTime());
    }
}

void EventlogFileManager::messageDeleted(cMessage *msg)
{
    if (isEventLogRecordingEnabled)
    {
        EventLogWriter::recordDeleteMessageEntry_id_pe(feventlog, msg->id(), msg->previousEventNumber());
    }
}

void EventlogFileManager::componentMethodBegin(cComponent *from, cComponent *to, const char *method)
{
    if (isEventLogRecordingEnabled)
    {
        if (from->isModule() && to->isModule())
        {
            EventLogWriter::recordModuleMethodBeginEntry_sm_tm_m(feventlog,
                ((cModule *)from)->id(), ((cModule *)to)->id(), method);
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

        bool recordModuleEvents = ev.config()->getAsBool(m->fullPath().c_str(), CFGID_RECORD_MODULE_EVENTS);
        m->setRecordEvents(recordModuleEvents);

        bool isCompoundModule = dynamic_cast<cCompoundModule *>(m);

        if (m->parentModule())
        {
            EventLogWriter::recordModuleCreatedEntry_id_c_pid_n_cm(feventlog,
                m->id(), m->className(), m->parentModule()->id(), m->fullName(), isCompoundModule); //FIXME size() is missing
        }
        else
        {
            EventLogWriter::recordModuleCreatedEntry_id_c_pid_n_cm(feventlog,
                m->id(), m->className(), -1, m->fullName(), isCompoundModule); //FIXME size() is missing; omit parentModuleId
        }
    }
}

void EventlogFileManager::moduleDeleted(cModule *module)
{
    if (isEventLogRecordingEnabled)
    {
        EventLogWriter::recordModuleDeletedEntry_id(feventlog, module->id());
    }
}

void EventlogFileManager::moduleReparented(cModule *module, cModule *oldparent)
{
    if (isEventLogRecordingEnabled)
    {
        EventLogWriter::recordModuleReparentedEntry_id_p(feventlog, module->id(), module->parentModule()->id());
    }
}

void EventlogFileManager::gateCreated(cGate *newgate)
{
    if (isEventLogRecordingEnabled)
    {
        EventLogWriter::recordGateCreatedEntry_m_g_n_i_o(feventlog, 
            newgate->ownerModule()->id(), newgate->id(), newgate->name(), newgate->index(), newgate->type() == cGate::OUTPUT);
    }
}

void EventlogFileManager::gateDeleted(cGate *gate)
{
    if (isEventLogRecordingEnabled)
    {
        EventLogWriter::recordGateDeletedEntry_m_g(feventlog, gate->ownerModule()->id(), gate->id());
    }
}

void EventlogFileManager::connectionCreated(cGate *srcgate)
{
    if (isEventLogRecordingEnabled)
    {
        cGate *destgate = srcgate->toGate();
        EventLogWriter::recordConnectionCreatedEntry_sm_sg_sn_dm_dg_dn(feventlog,
            srcgate->ownerModule()->id(), srcgate->id(), srcgate->fullName(),
            destgate->ownerModule()->id(), destgate->id(), destgate->fullName());  //XXX channel, channel attributes, etc
    }
}

void EventlogFileManager::connectionDeleted(cGate *srcgate)
{
    if (isEventLogRecordingEnabled)
    {
        EventLogWriter::recordConnectionDeletedEntry_sm_sg(feventlog,
            srcgate->ownerModule()->id(), srcgate->id());
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
                module->id(), module->displayString().str());
        }
        else if (dynamic_cast<cChannel *>(component))
        {
            cChannel *channel = (cChannel *)component;
            cGate *gate = channel->fromGate();
            EventLogWriter::recordConnectionDisplayStringChangedEntry_sm_sg_d(feventlog,
                gate->ownerModule()->id(), gate->id(), channel->displayString().str());
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

