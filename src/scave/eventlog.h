//=========================================================================
//  EVENTTRACE.H - part of
//                  OMNeT++/OMNEST
//           Discrete System Simulation in C++
//
//=========================================================================

/*--------------------------------------------------------------*
  Copyright (C) 1992-2005 Andras Varga

  This file is distributed WITHOUT ANY WARRANTY. See the file
  `license' for details on this and other legal matters.
*--------------------------------------------------------------*/

#ifndef __EVENTTRACE_H_
#define __EVENTTRACE_H_

#include <sstream>
#include <list>
#include <vector>
#include <map>
#include "filetokenizer.h"

/**
 * A compound or simple module extracted from the log file
 */
class ModuleEntry
{
    public:
        std::string moduleClassName;
        std::string moduleFullName;
        int moduleId;

    public:
        ModuleEntry();
        ~ModuleEntry();
};

typedef std::vector<ModuleEntry*> ModuleEntryList;

class EventEntry;

/**
 * A message sent from one module to another.
 */
class MessageEntry
{
    public:
        /** Tells if this entry represents a message delivery or a message send */
        bool isDelivery;
        long lineNumber;
        std::string messageClassName;
        std::string messageName;

         /** These log messages actually belong to the target event, but this way we can preserve ordering of message entries within the event */
         std::vector<std::string> logMessages;

        EventEntry *source;
        EventEntry *target;

    public:
        MessageEntry();
        ~MessageEntry();
};

typedef std::vector<MessageEntry*> MessageEntryList;

/**
 * An event log entry as seen in the log file corresponding to a handleMessage call.
 */
class EventEntry
{
    public:
        long eventNumber;
        double simulationTime;
        ModuleEntry *module;
        MessageEntry *cause;
        MessageEntryList causes;
        MessageEntryList consequences;
        int cachedX;  // Java code caches coordinates on chart here
        int cachedY;  // Java code caches coordinates on chart here
    public:
        EventEntry();
        ~EventEntry();
};

/**
 * A trace utility to trace the causes and consequences of a particular event back and forth in time.
 */
class EventLog
{
    protected:
        typedef std::vector<EventEntry*> EventEntryList;

        /** If this is a filtered event log, the parent log owns the event entries and message entries */
        EventLog *parent;
        /** Name of the file */;
        const char *logFileName;
        /** Modules that occur in the events */
        ModuleEntryList moduleList;
        /** Ordered list of EventEntries as read from the log file */
        EventEntryList eventList;
        /** Ordered list of MessageEntries as read from the log file */
        MessageEntryList messageList;
        /** Last traced event if any */
        EventEntry *tracedEvent;

    public:
        EventLog(const char *logFileName);
        ~EventLog();

        /**
         * Returns the number of events. Events are guaranteed to be in
         * increasing eventNumber and time order.
         */
        int getNumEvents();
        EventEntry *getEvent(int pos);
        int findEvent(EventEntry *event); // returns pos, or -1 if not found
        EventEntry *getEventByNumber(long eventNumber);
        EventEntry *getFirstEventAfter(double t);

        int getNumModules();
        ModuleEntry *getModule(int pos);

        EventLog *traceEvent(EventEntry *tracedEvent, bool wantCauses, bool wantConsequences);
        void writeTrace(FILE* fout);

    protected:
        EventLog(EventLog *parent);
        void parseLogFile();
        ModuleEntry *getOrAddModule(int moduleId, char *moduleClassName, char *moduleFullName);
        char *tokensToStr(int numTokens, char **vec);
};

/**
 * A class that makes it possible to extract info about events, without
 * returning objects. (Wherever a C++ method returns an object pointer,
 * SWIG-generated wrapper creates a corresponding Java object with the
 * pointer value inside. This has disastrous effect on performance
 * when dealing with huge amounts of data).
 *
 * IMPORTANT! Java code MUST NOT ask members of objects that don't exist!
 * Otherwise there will be a crash. For example, if getEvent_i_hasCause(i)
 * returns false, then getEvent_i_cause_messageClassName(i) will CRASH!
 */
class JavaFriendlyEventLogFacade
{
    protected:
        EventLog *log;

    protected:
        EventEntry *getEvent(int pos) {return log->getEvent(pos);}
        ModuleEntry *getEvent_module(int pos) {return log->getEvent(pos)->module;}
        MessageEntry *getEvent_cause(int pos) {return log->getEvent(pos)->cause;}
        MessageEntry *getEvent_causes(int pos, int k) {return log->getEvent(pos)->causes[k];}
        MessageEntry *getEvent_consequences(int pos, int k) {return log->getEvent(pos)->consequences[k];}

    public:
        JavaFriendlyEventLogFacade(EventLog *eventLog) {log = eventLog;}
        ~JavaFriendlyEventLogFacade() {}

        int getNumEvents() {return log->getNumEvents();}
        long getEvent_i_eventNumber(int pos) {return getEvent(pos)->eventNumber;}
        double getEvent_i_simulationTime(int pos) {return getEvent(pos)->simulationTime;}
        int getEvent_i_cachedX(int pos)  {return getEvent(pos)->cachedX;}
        int getEvent_i_cachedY(int pos)  {return getEvent(pos)->cachedY;}
        void setEvent_cachedX(int pos, int x)  {getEvent(pos)->cachedX = x;}
        void setEvent_cachedY(int pos, int y)  {getEvent(pos)->cachedY = y;}

        std::string getEvent_i_module_moduleClassName(int pos) {return getEvent_module(pos)->moduleClassName;}
        std::string getEvent_i_module_moduleFullName(int pos)  {return getEvent_module(pos)->moduleFullName;}
        int getEvent_i_module_moduleId(int pos) {return getEvent_module(pos)->moduleId;}

        bool getEvent_i_hasCause(int pos) {return getEvent_cause(pos)!=NULL;}
        bool getEvent_i_cause_isDelivery(int pos) {return getEvent_cause(pos)->isDelivery;}
        long getEvent_i_cause_lineNumber(int pos) {return getEvent_cause(pos)->lineNumber;}
        std::string getEvent_i_cause_messageClassName(int pos) {return getEvent_cause(pos)->messageClassName;}
        std::string getEvent_i_cause_messageName(int pos) {return getEvent_cause(pos)->messageName;}
        bool getEvent_i_cause_hasSourceAndTarget(int pos, int k)  {MessageEntry *m=getEvent_cause(pos); return m->source && m->target;}
        bool getEvent_i_cause_hasSource(int pos)  {return getEvent_cause(pos)->source!=NULL;}
        long getEvent_i_cause_source_eventNumber(int pos)  {return getEvent_cause(pos)->source->eventNumber;}
        double getEvent_i_cause_source_simulationTime(int pos)  {return getEvent_cause(pos)->source->simulationTime;}
        int getEvent_i_cause_source_module_moduleId(int pos, int k)  {return getEvent_cause(pos)->source->module->moduleId;}
        int getEvent_i_cause_source_cachedX(int pos)  {return getEvent_cause(pos)->source->cachedX;}
        int getEvent_i_cause_source_cachedY(int pos)  {return getEvent_cause(pos)->source->cachedY;}

        int getEvent_i_numCauses(int pos)  {return getEvent(pos)->causes.size();}

        bool getEvent_i_causes_k_isDelivery(int pos, int k) {return getEvent_causes(pos,k)->isDelivery;}
        long getEvent_i_causes_k_lineNumber(int pos, int k) {return getEvent_causes(pos,k)->lineNumber;}
        std::string getEvent_i_causes_k_messageClassName(int pos, int k) {return getEvent_causes(pos,k)->messageClassName;}
        std::string getEvent_i_causes_k_messageName(int pos, int k) {return getEvent_causes(pos,k)->messageName;}
        bool getEvent_i_causes_k_hasSourceAndTarget(int pos, int k)  {MessageEntry *m=getEvent_causes(pos,k); return m->source && m->target;}
        bool getEvent_i_causes_k_hasSource(int pos, int k)  {return getEvent_causes(pos,k)->source!=NULL;}
        long getEvent_i_causes_k_source_eventNumber(int pos, int k)  {return getEvent_causes(pos,k)->source->eventNumber;}
        double getEvent_i_causes_k_source_simulationTime(int pos, int k)  {return getEvent_causes(pos,k)->source->simulationTime;}
        int getEvent_i_causes_k_source_module_moduleId(int pos, int k)  {return getEvent_causes(pos,k)->source->module->moduleId;}
        int getEvent_i_causes_k_source_cachedX(int pos, int k)  {return getEvent_causes(pos,k)->source->cachedX;}
        int getEvent_i_causes_k_source_cachedY(int pos, int k)  {return getEvent_causes(pos,k)->source->cachedY;}
        bool getEvent_i_causes_k_hasTarget(int pos, int k)  {return getEvent_causes(pos,k)->target!=NULL;}
        long getEvent_i_causes_k_target_eventNumber(int pos, int k)  {return getEvent_causes(pos,k)->target->eventNumber;}
        double getEvent_i_causes_k_target_simulationTime(int pos, int k)  {return getEvent_causes(pos,k)->target->simulationTime;}
        int getEvent_i_causes_k_target_module_moduleId(int pos, int k)  {return getEvent_causes(pos,k)->target->module->moduleId;}
        int getEvent_i_causes_k_target_cachedX(int pos, int k)  {return getEvent_causes(pos,k)->target->cachedX;}
        int getEvent_i_causes_k_target_cachedY(int pos, int k)  {return getEvent_causes(pos,k)->target->cachedY;}

        int getEvent_i_numConsequences(int pos)  {return getEvent(pos)->consequences.size();}

        bool getEvent_i_consequences_k_isDelivery(int pos, int k) {return getEvent_consequences(pos,k)->isDelivery;}
        long getEvent_i_consequences_k_lineNumber(int pos, int k) {return getEvent_consequences(pos,k)->lineNumber;}
        std::string getEvent_i_consequences_k_messageClassName(int pos, int k) {return getEvent_consequences(pos,k)->messageClassName;}
        std::string getEvent_i_consequences_k_messageName(int pos, int k) {return getEvent_consequences(pos,k)->messageName;}
        bool getEvent_i_consequences_k_hasSourceAndTarget(int pos, int k)  {MessageEntry *m=getEvent_consequences(pos,k); return m->source && m->target;}
        bool getEvent_i_consequences_k_hasSource(int pos, int k)  {return getEvent_consequences(pos,k)->source!=NULL;}
        long getEvent_i_consequences_k_source_eventNumber(int pos, int k)  {return getEvent_consequences(pos,k)->source->eventNumber;}
        double getEvent_i_consequences_k_source_simulationTime(int pos, int k)  {return getEvent_consequences(pos,k)->source->simulationTime;}
        int getEvent_i_consequences_k_source_module_moduleId(int pos, int k)  {return getEvent_consequences(pos,k)->source->module->moduleId;}
        int getEvent_i_consequences_k_source_cachedX(int pos, int k)  {return getEvent_consequences(pos,k)->source->cachedX;}
        int getEvent_i_consequences_k_source_cachedY(int pos, int k)  {return getEvent_consequences(pos,k)->source->cachedY;}
        bool getEvent_i_consequences_k_hasTarget(int pos, int k)  {return getEvent_consequences(pos,k)->target!=NULL;}
        long getEvent_i_consequences_k_target_eventNumber(int pos, int k)  {return getEvent_consequences(pos,k)->target->eventNumber;}
        double getEvent_i_consequences_k_target_simulationTime(int pos, int k)  {return getEvent_consequences(pos,k)->target->simulationTime;}
        int getEvent_i_consequences_k_target_module_moduleId(int pos, int k)  {return getEvent_consequences(pos,k)->target->module->moduleId;}
        int getEvent_i_consequences_k_target_cachedX(int pos, int k)  {return getEvent_consequences(pos,k)->target->cachedX;}
        int getEvent_i_consequences_k_target_cachedY(int pos, int k)  {return getEvent_consequences(pos,k)->target->cachedY;}
};

#endif


