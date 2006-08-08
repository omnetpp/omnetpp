//=========================================================================
//  EVENTLOG.H - part of
//                  OMNeT++/OMNEST
//           Discrete System Simulation in C++
//
//=========================================================================

/*--------------------------------------------------------------*
  Copyright (C) 1992-2005 Andras Varga

  This file is distributed WITHOUT ANY WARRANTY. See the file
  `license' for details on this and other legal matters.
*--------------------------------------------------------------*/

#ifndef __EVENTLOG_H_
#define __EVENTLOG_H_

#include <sstream>
#include <list>
#include <vector>
#include <map>
#include <set>
#include "filetokenizer.h"
#include "exception.h"
#include "eventlogdefs.h"

/**
 * A compound or simple module extracted from the log file
 */
class ModuleEntry
{
    public:
        const char *moduleClassName; // stringpooled
        std::string moduleFullPath;  // unique, no need for stringpool
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
        bool isDelivery; // if true, this message is the target event's cause (this==source->cause)
        long lineNumber; // in the event log file
        const char *messageClassName; // stringpooled
        const char *messageName;  // stringpooled
        ModuleEntry *module; // this is the target event's module (unless there's Enter_Method() involved)

        /** These log messages actually belong to the target event, but this way we can preserve ordering of message entries within the event */
        std::vector<const char *> logMessages; // stringpooled

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
        MessageEntry *cause;  // the message handled in this event (i.e. passed to handleMessage())
        MessageEntryList causes;  // also includes "cause"
        MessageEntryList consequences;
        int numLogMessages; // total number of log messages for this event (sum of its causes[]' log messages)

        // temporary state for tracing events, coloring (graph)
        bool isInCollectedEvents;

        // the following fields are for the convenience of the GUI
        double timelineCoordinate;
        int64 cachedX;
        int64 cachedY;
        bool isExpandedInTree;
        int tableRowIndex;

    public:
        EventEntry();
        ~EventEntry();
};

/**
 * For saving memory on the storage of (largely) constant strings that occur in
 * many instances. (See Flyweight GoF pattern.)
 */
class StringPool
{
  protected:
    struct strless {
        bool operator()(const char *s1, const char *s2) const {
            int d0 = *s1 - *s2;
            if (d0<0) return true; else if (d0>0) return false;
            return strcmp(s1+1,s2+1)<0;
        }
    };
    typedef std::set<char *,strless> StringSet;
    StringSet pool;

  public:
    StringPool();
    ~StringPool();
    const char *get(const char *s);
};

/**
 * A trace utility to trace the causes and consequences of a particular event back and forth in time.
 */
class EventLog
{
    protected:
        typedef std::vector<EventEntry*> EventEntryList;

        std::vector<const char *> buildLogMessages; // stringpooled
        std::vector<const char *> initializeLogMessages; // stringpooled
        std::vector<const char *> finishLogMessages; // stringpooled

        /** If this is a filtered event log, the parent log owns the event entries and message entries */
        EventLog *parent;
        /** Name of the file */;
        std::string logFileName;
        /** Modules that occur in the events */
        ModuleEntryList moduleList;
        /** Ordered list of EventEntries as read from the log file */
        EventEntryList eventList;
        /** Ordered list of MessageEntries as read from the log file */
        MessageEntryList messageList;
        /** Last traced event if any */
        EventEntry *tracedEvent;

        /** String storage */
        StringPool stringPool;

    public:
        EventLog(const char *logFileName);
        ~EventLog();

        /**
         * Returns the number of events. Events are guaranteed to be in
         * increasing eventNumber and time order.
         */
        int getNumEvents();
        EventEntry *getEvent(int pos);
        EventEntry *getFirstEvent(); // convenience method
        EventEntry *getLastEvent(); // convenience method
        int findEvent(EventEntry *event); // returns pos, or -1 if not found
        bool containsEvent(EventEntry *event) {return findEvent(event)!=-1;}
        inline int getEventPositionByNumber(long eventNumber);
        EventEntry *getEventByNumber(long eventNumber);
        inline EventEntry *getEventNULLSafe(int pos);
        inline int getFirstEventPositionAfterByPredicate(double t, bool (*predicate)(EventEntry*,double));
        inline int getLastEventPositionBeforeByPredicate(double t, bool (*predicate)(EventEntry*,double));
        int getFirstEventPositionAfter(double t);
        int getLastEventPositionBefore(double t);
        EventEntry *getFirstEventAfter(double t);
        EventEntry *getLastEventBefore(double t);
        int getFirstEventPositionAfterByTimelineCoordinate(double t);
        int getLastEventPositionBeforeByTimelineCoordinate(double t);
        EventEntry *getFirstEventAfterByTimelineCoordinate(double t);
        EventEntry *getLastEventBeforeByTimelineCoordinate(double t);
        EventEntry *getEventByTableRowIndex(int tableRowIndex);

        int getNumMessages();
        MessageEntry *getMessage(int pos);

        /**
         * Collect messages whose sourceEventNum<end and targetEventNum>=start,
         * and both modules are in the given set (if given, as moduleIds may be NULL).
         * Message indices are returned in the result vector, which has to be
         * deallocated by the caller.
         */
        std::vector<int> *getMessagesIntersecting(long startEventNumber, long endEventNumber, std::set<int> *moduleIds, bool wantNonDeliveryMsgs);

        int getNumModules();
        ModuleEntry *getModule(int pos);

        // manipulating GUI vars stored in EventEntry
        void expandAllEvents();
        void collapseAllEvents();
        void expandEvent(int pos);
        void collapseEvent(int pos);

        /**
         * Filters an event log to the causes/consequences of one given event;
         * only events/messages whose module is in the given set are collected.
         * moduleIds may be NULL.
         */
        EventLog *traceEvent(EventEntry *tracedEvent, std::set<int> *moduleIds, bool wantCauses, bool wantConsequences, bool wantNonDeliveryMessages);

        /**
         * Returns an "adjacency matrix" of the nodes, the "edge cost" being the number
         * of messages from node1 to node2. This is used for ordering axes on the GUI
         * (node==axis; and there can be several modules on each axis). The returned vector
         * is of size n*n, n being the number of nodes; it has to be deallocated by the caller.
         */
        std::vector<int> *buildMessageCountGraph(std::map<int, int> *moduleIdToNodeIdMap);  //XXX use const std::map&

        /** Records the eventlog to a file */
        void writeTrace(FILE* fout);

    protected:
        EventLog(EventLog *parent);
        void parseLogFile();
        ModuleEntry *getOrAddModule(int moduleId, char *moduleClassName, char *moduleFullPath);
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
        ModuleEntry *getEvent_cause_module(int pos) {return log->getEvent(pos)->cause->module;}
        MessageEntry *getEvent_cause(int pos) {return log->getEvent(pos)->cause;}
        MessageEntry *getEvent_causes(int pos, int k) {
            EventEntry *e = log->getEvent(pos);
            if (k<0 || k>=e->causes.size())
                throw new Exception("causes[] index %d of event #%ld out of bounds", pos, e->eventNumber);
            return e->causes[k];
        }
        MessageEntry *getEvent_consequences(int pos, int k) {
            EventEntry *e = log->getEvent(pos);
            if (k<0 || k>=e->consequences.size())
                throw new Exception("getEvent_consequences[] index %d of event #%ld out of bounds", pos, e->eventNumber);
            return e->consequences[k];
        }
        MessageEntry *getMessage(int pos) {return log->getMessage(pos);}

    public:
        JavaFriendlyEventLogFacade(EventLog *eventLog) {log = eventLog;}
        ~JavaFriendlyEventLogFacade() {}

        int getNumEvents() {return log->getNumEvents();}
        long getEvent_i_eventNumber(int pos) {return getEvent(pos)->eventNumber;}
        double getEvent_i_simulationTime(int pos) {return getEvent(pos)->simulationTime;}
        int64 getEvent_i_cachedX(int pos)  {return getEvent(pos)->cachedX;}
        int64 getEvent_i_cachedY(int pos)  {return getEvent(pos)->cachedY;}
        bool getEvent_i_isExpandedInTree(int pos)  {return getEvent(pos)->isExpandedInTree;}
        int getEvent_i_tableRowIndex(int pos)  {return getEvent(pos)->tableRowIndex;}
        double getEvent_i_timelineCoordinate(int pos)  {return getEvent(pos)->timelineCoordinate;}
        void setEvent_cachedX(int pos, int64 x)  {getEvent(pos)->cachedX = x;}
        void setEvent_cachedY(int pos, int64 y)  {getEvent(pos)->cachedY = y;}
        void setEvent_i_isExpandedInTree(int pos, bool exp)  {getEvent(pos)->isExpandedInTree = exp;} //XXX modify tableRowIndex!!!
        void setEvent_i_timelineCoordinate(int pos, double x)  {getEvent(pos)->timelineCoordinate = x;}

        const char *getEvent_i_module_moduleClassName(int pos) {return getEvent_cause_module(pos)->moduleClassName;}
        const char *getEvent_i_module_moduleFullPath(int pos)  {return getEvent_cause_module(pos)->moduleFullPath.c_str();}
        int getEvent_i_module_moduleId(int pos) {return getEvent_cause_module(pos)->moduleId;}

        bool getEvent_i_hasCause(int pos) {return getEvent_cause(pos)!=NULL;}
        bool getEvent_i_cause_isDelivery(int pos) {return getEvent_cause(pos)->isDelivery;}
        long getEvent_i_cause_lineNumber(int pos) {return getEvent_cause(pos)->lineNumber;}
        const char *getEvent_i_cause_messageClassName(int pos) {return getEvent_cause(pos)->messageClassName;}
        const char *getEvent_i_cause_messageName(int pos) {return getEvent_cause(pos)->messageName;}

        bool getEvent_i_cause_hasSource(int pos)  {return getEvent_cause(pos)->source!=NULL;}
        bool getEvent_i_cause_source_isInFilteredSubset(int pos, EventLog *filteredSubset)  {return filteredSubset->containsEvent(getEvent_cause(pos)->source);}
        long getEvent_i_cause_source_eventNumber(int pos)  {return getEvent_cause(pos)->source->eventNumber;}
        double getEvent_i_cause_source_simulationTime(int pos)  {return getEvent_cause(pos)->source->simulationTime;}
        int getEvent_i_cause_source_cause_module_moduleId(int pos, int k)  {return getEvent_cause(pos)->source->cause->module->moduleId;}
        int64 getEvent_i_cause_source_cachedX(int pos)  {return getEvent_cause(pos)->source->cachedX;}
        int64 getEvent_i_cause_source_cachedY(int pos)  {return getEvent_cause(pos)->source->cachedY;}
        void setEvent_i_cause_source_cachedX(int pos, int64 x)  {getEvent_cause(pos)->source->cachedX = x;}
        void setEvent_i_cause_source_cachedY(int pos, int64 y)  {getEvent_cause(pos)->source->cachedY = y;}

        int getEvent_i_numCauses(int pos)  {return getEvent(pos)->causes.size();}

        bool getEvent_i_causes_k_isDelivery(int pos, int k) {return getEvent_causes(pos,k)->isDelivery;}
        long getEvent_i_causes_k_lineNumber(int pos, int k) {return getEvent_causes(pos,k)->lineNumber;}
        const char *getEvent_i_causes_k_messageClassName(int pos, int k) {return getEvent_causes(pos,k)->messageClassName;}
        const char *getEvent_i_causes_k_messageName(int pos, int k) {return getEvent_causes(pos,k)->messageName;}
        bool getEvent_i_causes_k_hasSource(int pos, int k)  {return getEvent_causes(pos,k)->source!=NULL;}
        bool getEvent_i_causes_k_source_isInFilteredSubset(int pos, int k, EventLog *filteredSubset)  {return filteredSubset->containsEvent(getEvent_causes(pos,k)->source);}
        long getEvent_i_causes_k_source_eventNumber(int pos, int k)  {return getEvent_causes(pos,k)->source->eventNumber;}
        double getEvent_i_causes_k_source_simulationTime(int pos, int k)  {return getEvent_causes(pos,k)->source->simulationTime;}
        double getEvent_i_causes_k_source_timelineCoordinate(int pos, int k)  {return getEvent_causes(pos,k)->source->timelineCoordinate;}
        int getEvent_i_causes_k_source_cause_module_moduleId(int pos, int k)  {return getEvent_causes(pos,k)->source->cause->module->moduleId;}
        int64 getEvent_i_causes_k_source_cachedX(int pos, int k)  {return getEvent_causes(pos,k)->source->cachedX;}
        int64 getEvent_i_causes_k_source_cachedY(int pos, int k)  {return getEvent_causes(pos,k)->source->cachedY;}
        void setEvent_i_causes_k_source_cachedX(int pos, int k, int64 x)  {getEvent_causes(pos,k)->source->cachedX = x;}
        void setEvent_i_causes_k_source_cachedY(int pos, int k, int64 y)  {getEvent_causes(pos,k)->source->cachedY = y;}

        int getEvent_i_numConsequences(int pos)  {return getEvent(pos)->consequences.size();}

        bool getEvent_i_consequences_k_isDelivery(int pos, int k) {return getEvent_consequences(pos,k)->isDelivery;}
        long getEvent_i_consequences_k_lineNumber(int pos, int k) {return getEvent_consequences(pos,k)->lineNumber;}
        const char *getEvent_i_consequences_k_messageClassName(int pos, int k) {return getEvent_consequences(pos,k)->messageClassName;}
        const char *getEvent_i_consequences_k_messageName(int pos, int k) {return getEvent_consequences(pos,k)->messageName;}
        bool getEvent_i_consequences_k_hasTarget(int pos, int k)  {return getEvent_consequences(pos,k)->target!=NULL;}
        bool getEvent_i_consequences_k_target_isInFilteredSubset(int pos, int k, EventLog *filteredSubset)  {return filteredSubset->containsEvent(getEvent_consequences(pos,k)->target);}
        long getEvent_i_consequences_k_target_eventNumber(int pos, int k)  {return getEvent_consequences(pos,k)->target->eventNumber;}
        double getEvent_i_consequences_k_target_simulationTime(int pos, int k)  {return getEvent_consequences(pos,k)->target->simulationTime;}
        double getEvent_i_consequences_k_target_timelineCoordinatea(int pos, int k)  {return getEvent_consequences(pos,k)->target->timelineCoordinate;}
        int getEvent_i_consequences_k_target_cause_module_moduleId(int pos, int k)  {return getEvent_consequences(pos,k)->target->cause->module->moduleId;}
        int64 getEvent_i_consequences_k_target_cachedX(int pos, int k)  {return getEvent_consequences(pos,k)->target->cachedX;}
        int64 getEvent_i_consequences_k_target_cachedY(int pos, int k)  {return getEvent_consequences(pos,k)->target->cachedY;}
        void setEvent_i_consequences_k_target_cachedX(int pos, int k, int64 x)  {getEvent_consequences(pos,k)->target->cachedX = x;}
        void setEvent_i_consequences_k_target_cachedY(int pos, int k, int64 y)  {getEvent_consequences(pos,k)->target->cachedY = y;}

        int getNumMessages() {return log->getNumMessages();}

        bool getMessage_isDelivery(int pos) {return getMessage(pos)->isDelivery;}
        long getMessage_lineNumber(int pos) {return getMessage(pos)->lineNumber;}
        const char *getMessage_messageClassName(int pos) {return getMessage(pos)->messageClassName;}
        const char *getMessage_messageName(int pos) {return getMessage(pos)->messageName;}

        const char *getMessage_module_moduleClassName(int pos) {return getMessage(pos)->module->moduleClassName;}
        const char *getMessage_module_moduleFullPath(int pos)  {return getMessage(pos)->module->moduleFullPath.c_str();}
        int getMessage_module_moduleId(int pos) {return getMessage(pos)->module->moduleId;}

        bool getMessage_hasSource(int pos)  {return getMessage(pos)->source!=NULL;}
        bool getMessage_source_isInFilteredSubset(int pos, EventLog *filteredSubset)  {return filteredSubset->containsEvent(getMessage(pos)->source);}
        long getMessage_source_eventNumber(int pos)  {return getMessage(pos)->source->eventNumber;}
        double getMessage_source_simulationTime(int pos)  {return getMessage(pos)->source->simulationTime;}
        double getMessage_source_timelineCoordinate(int pos)  {return getMessage(pos)->source->timelineCoordinate;}
        int getMessage_source_cause_module_moduleId(int pos)  {return getMessage(pos)->source->cause->module->moduleId;}
        int64 getMessage_source_cachedX(int pos)  {return getMessage(pos)->source->cachedX;}
        int64 getMessage_source_cachedY(int pos)  {return getMessage(pos)->source->cachedY;}
        void setMessage_source_cachedX(int pos, int64 x)  {getMessage(pos)->source->cachedX = x;}
        void setMessage_source_cachedY(int pos, int64 y)  {getMessage(pos)->source->cachedY = y;}

        bool getMessage_hasTarget(int pos)  {return getMessage(pos)->target!=NULL;}
        bool getMessage_target_isInFilteredSubset(int pos, EventLog *filteredSubset)  {return filteredSubset->containsEvent(getMessage(pos)->target);}
        long getMessage_target_eventNumber(int pos)  {return getMessage(pos)->target->eventNumber;}
        double getMessage_target_simulationTime(int pos)  {return getMessage(pos)->target->simulationTime;}
        double getMessage_target_timelineCoordinate(int pos)  {return getMessage(pos)->target->timelineCoordinate;}
        int getMessage_target_cause_module_moduleId(int pos)  {return getMessage(pos)->target->cause->module->moduleId;}
        int64 getMessage_target_cachedX(int pos)  {return getMessage(pos)->target->cachedX;}
        int64 getMessage_target_cachedY(int pos)  {return getMessage(pos)->target->cachedY;}
        void setMessage_target_cachedX(int pos, int64 x)  {getMessage(pos)->target->cachedX = x;}
        void setMessage_target_cachedY(int pos, int64 y)  {getMessage(pos)->target->cachedY = y;}
};

#endif


