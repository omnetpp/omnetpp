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
 * pointer value inside. This is disastrous when dealing with huge amounts
 * of data).
 *
 * IMPORTANT! Java code MUST NOT ask members of objects that don't exist!
 * Otherwise there will be a crash. For example, if getEvent_k_hasCause(k)
 * returns false, then getEvent_k_cause_messageClassName(k) will CRASH!
 */
class JavaFriendlyEventLogFacade
{
    protected:
        EventLog *log;

    protected:
        EventEntry *getEvent(int pos) {return log->getEvent(pos);}
        ModuleEntry *getEvent_module(int pos) {return log->getEvent(pos)->module;}
        MessageEntry *getEvent_cause(int pos) {return log->getEvent(pos)->cause;}
        MessageEntry *getEvent_causes(int pos, int i) {return log->getEvent(pos)->causes[i];}
        MessageEntry *getEvent_consequences(int pos, int i) {return log->getEvent(pos)->consequences[i];}

    public:
        JavaFriendlyEventLogFacade(EventLog *eventLog) {log = eventLog;}
        ~JavaFriendlyEventLogFacade() {}

        int getNumEvents() {return log->getNumEvents();}
        long getEvent_k_eventNumber(int pos) {return getEvent(pos)->eventNumber;}
        double getEvent_k_simulationTime(int pos) {return getEvent(pos)->simulationTime;}
        int getEvent_k_cachedX(int pos)  {return getEvent(pos)->cachedX;}
        int getEvent_k_cachedY(int pos)  {return getEvent(pos)->cachedY;}
        void setEvent_cachedX(int pos, int x)  {getEvent(pos)->cachedX = x;}
        void setEvent_cachedY(int pos, int y)  {getEvent(pos)->cachedY = y;}

        std::string getEvent_k_module_moduleClassName(int pos) {return getEvent_module(pos)->moduleClassName;}
        std::string getEvent_k_module_moduleFullName(int pos)  {return getEvent_module(pos)->moduleFullName;}
        int getEvent_k_module_moduleId(int pos) {return getEvent_module(pos)->moduleId;}

        bool getEvent_k_hasCause(int pos) {return getEvent_cause(pos)!=NULL;}
        bool getEvent_k_cause_isDelivery(int pos) {return getEvent_cause(pos)->isDelivery;}
        long getEvent_k_cause_lineNumber(int pos) {return getEvent_cause(pos)->lineNumber;}
        std::string getEvent_k_cause_messageClassName(int pos) {return getEvent_cause(pos)->messageClassName;}
        std::string getEvent_k_cause_messageName(int pos) {return getEvent_cause(pos)->messageName;}
        bool getEvent_k_cause_hasSource(int pos)  {return getEvent_cause(pos)->source!=NULL;}
        double getEvent_k_cause_source_simulationTime(int pos)  {return getEvent_cause(pos)->source->simulationTime;}
        int getEvent_k_cause_source_cachedX(int pos)  {return getEvent_cause(pos)->source->cachedX;}
        int getEvent_k_cause_source_cachedY(int pos)  {return getEvent_cause(pos)->source->cachedY;}

        double getEvent_k_numCauses(int pos)  {return getEvent(pos)->causes.size();}

        bool getEvent_k_causes_i_isDelivery(int pos, int i) {return getEvent_causes(pos,i)->isDelivery;}
        long getEvent_k_causes_i_lineNumber(int pos, int i) {return getEvent_causes(pos,i)->lineNumber;}
        std::string getEvent_k_causes_i_messageClassName(int pos, int i) {return getEvent_causes(pos,i)->messageClassName;}
        std::string getEvent_k_causes_i_messageName(int pos, int i) {return getEvent_causes(pos,i)->messageName;}
        bool getEvent_k_causes_i_hasSource(int pos, int i)  {return getEvent_causes(pos,i)->source!=NULL;}
        double getEvent_k_causes_i_source_simulationTime(int pos, int i)  {return getEvent_causes(pos,i)->source->simulationTime;}
        int getEvent_k_causes_i_source_cachedX(int pos, int i)  {return getEvent_causes(pos,i)->source->cachedX;}
        int getEvent_k_causes_i_source_cachedY(int pos, int i)  {return getEvent_causes(pos,i)->source->cachedY;}
        bool getEvent_k_causes_i_hasTarget(int pos, int i)  {return getEvent_causes(pos,i)->target!=NULL;}
        double getEvent_k_causes_i_target_simulationTime(int pos, int i)  {return getEvent_causes(pos,i)->target->simulationTime;}
        int getEvent_k_causes_i_target_cachedX(int pos, int i)  {return getEvent_causes(pos,i)->target->cachedX;}
        int getEvent_k_causes_i_target_cachedY(int pos, int i)  {return getEvent_causes(pos,i)->target->cachedY;}

        double getEvent_k_numConsequences(int pos)  {return getEvent(pos)->consequences.size();}

        bool getEvent_k_consequences_i_isDelivery(int pos, int i) {return getEvent_consequences(pos,i)->isDelivery;}
        long getEvent_k_consequences_i_lineNumber(int pos, int i) {return getEvent_consequences(pos,i)->lineNumber;}
        std::string getEvent_k_consequences_i_messageClassName(int pos, int i) {return getEvent_consequences(pos,i)->messageClassName;}
        std::string getEvent_k_consequences_i_messageName(int pos, int i) {return getEvent_consequences(pos,i)->messageName;}
        bool getEvent_k_consequences_i_hasSource(int pos, int i)  {return getEvent_consequences(pos,i)->source!=NULL;}
        double getEvent_k_consequences_i_source_simulationTime(int pos, int i)  {return getEvent_consequences(pos,i)->source->simulationTime;}
        int getEvent_k_consequences_i_source_cachedX(int pos, int i)  {return getEvent_consequences(pos,i)->source->cachedX;}
        int getEvent_k_consequences_i_source_cachedY(int pos, int i)  {return getEvent_consequences(pos,i)->source->cachedY;}
        bool getEvent_k_consequences_i_hasTarget(int pos, int i)  {return getEvent_consequences(pos,i)->target!=NULL;}
        double getEvent_k_consequences_i_target_simulationTime(int pos, int i)  {return getEvent_consequences(pos,i)->target->simulationTime;}
        int getEvent_k_consequences_i_target_cachedX(int pos, int i)  {return getEvent_consequences(pos,i)->target->cachedX;}
        int getEvent_k_consequences_i_target_cachedY(int pos, int i)  {return getEvent_consequences(pos,i)->target->cachedY;}
};

#endif


