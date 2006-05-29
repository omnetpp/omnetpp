//=========================================================================
//  EVENTLOG.CC - part of
//                  OMNeT++/OMNEST
//           Discrete System Simulation in C++
//
//=========================================================================

/*--------------------------------------------------------------*
  Copyright (C) 1992-2005 Andras Varga

  This file is distributed WITHOUT ANY WARRANTY. See the file
  `license' for details on this and other legal matters.
*--------------------------------------------------------------*/

#include <algorithm>
#include <set>
#include <assert.h>
#include "eventlog.h"

/*--------------------------------------------------------------*/
ModuleEntry::ModuleEntry()
{
    moduleId = -1;
    moduleClassName = ""; // must not be NULL because SWIG-generated code will crash!
}

ModuleEntry::~ModuleEntry()
{
}

/*--------------------------------------------------------------*/
EventEntry::EventEntry()
{
    eventNumber = -1;
    simulationTime = -1;
    module = NULL;
    cause = NULL;
    numLogMessages = 0;

    cachedX = cachedY = 0;
    isSelected = false;
    isExpandedInTree = false;
    tableRowIndex = 0;
}

EventEntry::~EventEntry()
{
}

/*--------------------------------------------------------------*/
MessageEntry::MessageEntry()
{
    isDelivery = false;
    lineNumber = -1;
    messageClassName = "";  // must not be NULL because SWIG-generated code will crash!
    messageName = ""; // must not be NULL because SWIG-generated code will crash!
    source = target = NULL;
}

MessageEntry::~MessageEntry()
{
}

/*--------------------------------------------------------------*/
StringPool::StringPool()
{
}

StringPool::~StringPool()
{
    for (StringSet::iterator it = pool.begin(); it!=pool.end(); ++it)
        delete [] *it;
    //XXX this one may be faster, test:
    //while (pool.size()>0) {
    //    delete [] *pool.begin();
    //    pool.erase(pool.begin());
    //}
}

const char *StringPool::get(const char *s)
{
    if (s==NULL)
        return ""; // must not be NULL because SWIG-generated code will crash!
    StringSet::iterator it = pool.find(const_cast<char *>(s));
    if (it!=pool.end())
        return *it;
    char *str = new char[strlen(s)+1];
    strcpy(str,s);
    pool.insert(str);
    return str;
}

/*--------------------------------------------------------------*/
EventLog::EventLog(const char *logFileName)
{
    this->logFileName = logFileName;
    parent = NULL;
    tracedEvent = NULL;

    parseLogFile();
}

EventLog::EventLog(EventLog *parent)
{
    this->parent = parent;
    tracedEvent = tracedEvent;
}

EventLog::~EventLog()
{
    if (!parent)
    {
        for (ModuleEntryList::iterator it = moduleList.begin(); it != moduleList.end(); it++)
            delete *it;

        for (EventEntryList::iterator it = eventList.begin(); it != eventList.end(); it++)
            delete *it;

        for (MessageEntryList::iterator it = messageList.begin(); it != messageList.end(); it++)
            delete *it;
    }
}

void EventLog::parseLogFile()
{
    long lineNumber = 0;
    MessageEntry *messageEntry = NULL;
    EventEntry *eventEntry = NULL;
    FileTokenizer ftok(logFileName.c_str());

    // read messages and events
    while (ftok.readLine())
    {
        int numTokens = ftok.numTokens();
        char **vec = ftok.tokens();
        lineNumber++;

        if (numTokens > 0)
        {
            if (!strcmp(vec[0], "*") || !strcmp(vec[0], "+"))
            {
                if (numTokens < 8)
                {
                    fprintf(stderr, "Invalid format at line %ld: %s\n", lineNumber, tokensToStr(numTokens, vec)); //XXX
                }
                else
                {
                    bool isDelivery = !strcmp(vec[0], "*");   //FIXME stricter format check overall
                    // skip [ character
                    long eventNumber = atol(vec[1] + 1);
                    long causalEventNumber = atol(vec[2]);

                    messageEntry = new MessageEntry();
                    messageEntry->lineNumber = lineNumber;
                    messageEntry->isDelivery = isDelivery;

                    if (isDelivery)
                    {
                        eventEntry = new EventEntry();
                        eventEntry->eventNumber = eventNumber;
                        eventEntry->simulationTime = atof(vec[3]);
                        // skip (id=
                        eventEntry->module = getOrAddModule(atoi(vec[6] + 4), vec[4], vec[5]);
                        eventEntry->cause = messageEntry;
                        eventList.push_back(eventEntry);
                    }

                    messageEntry->source = getEventByNumber(causalEventNumber);
                    messageEntry->target = getEventByNumber(eventNumber);
                    // skip () characters
                    *(vec[7] + strlen(vec[7]) - 1) = '\0';
                    messageEntry->messageClassName = stringPool.get(vec[7] + 1);

                    if (numTokens == 9)
                        messageEntry->messageName = stringPool.get(vec[8]);

                    messageList.push_back(messageEntry);
                }
            }
            else
            {
                // skip [ character
                long eventNumber = atol(vec[0] + 1);

                if (messageEntry != NULL && messageEntry->target->eventNumber == eventNumber)
                {
                    char *str = tokensToStr(numTokens - 1, vec + 1);
                    messageEntry->logMessages.push_back(stringPool.get(str));
                    delete [] str;
                }
            }
        }
    }

    // indexing: collect causes[] and consequences[] for each event; also calculate numLogMessages
    for (MessageEntryList::iterator it = messageList.begin(); it != messageList.end(); it++)
    {
        MessageEntry *messageEntry = *it;
        if (!messageEntry->source || !messageEntry->target)
        {
            fprintf(stderr, "Internal error (?): message at line %ld does not have source or target event set\n", lineNumber); //XXX
            continue;
        }
        messageEntry->source->consequences.push_back(messageEntry);
        messageEntry->target->causes.push_back(messageEntry);
        messageEntry->target->numLogMessages += messageEntry->logMessages.size();
    }
}

int EventLog::getNumEvents()
{
    return eventList.size();
}

EventEntry *EventLog::getEvent(int pos)
{
    if (pos<0 || pos>=eventList.size() || !eventList[pos])
        return NULL;
    return eventList[pos];
}

EventEntry *EventLog::getLastEvent()
{
    return eventList.empty() ? NULL : eventList.back();
}

int EventLog::getNumModules()
{
    return moduleList.size();
}

ModuleEntry *EventLog::getModule(int pos)
{
    if (pos<0 || pos>=moduleList.size() || !moduleList[pos])
        return NULL;
    return moduleList[pos];
}

int EventLog::getNumMessages()
{
    return messageList.size();
}

MessageEntry *EventLog::getMessage(int pos)
{
    if (pos<0 || pos>=messageList.size() || !messageList[pos])
        return NULL;
    return messageList[pos];
}

ModuleEntry *EventLog::getOrAddModule(int moduleId, char *moduleClassName, char *moduleFullPath)
{
    // if module with such ID already exists, return it
    //FIXME warn if className or fullPath doesn't match!
    for (ModuleEntryList::iterator it = moduleList.begin(); it != moduleList.end(); it++)
        if ((*it)->moduleId == moduleId)
            return *it;

    // not yet in there -- store it
    ModuleEntry *moduleEntry = new ModuleEntry();
    moduleEntry->moduleId = moduleId;
    // skip () characters
    *(moduleClassName + strlen(moduleClassName) - 1) = '\0';
    moduleEntry->moduleClassName = stringPool.get(moduleClassName + 1);
    moduleEntry->moduleFullPath = moduleFullPath;
    moduleList.push_back(moduleEntry);

    return moduleEntry;
}

inline bool less_EventEntry_long(EventEntry *e, long eventNumber) {return e->eventNumber < eventNumber;}

int EventLog::findEvent(EventEntry *event)
{
    EventEntryList::iterator it = std::lower_bound(eventList.begin(), eventList.end(), event->eventNumber, less_EventEntry_long);
    return (it!=eventList.end() && *it==event) ? it-eventList.begin() : -1;
}

EventEntry *EventLog::getEventByNumber(long eventNumber)
{
    EventEntryList::iterator it = std::lower_bound(eventList.begin(), eventList.end(), eventNumber, less_EventEntry_long);
    return it==eventList.end() ? NULL : *it;
}

inline bool less_EventEntry_double(EventEntry *e, double t) {return e->simulationTime < t;}

EventEntry *EventLog::getFirstEventAfter(double t)
{
    EventEntryList::iterator it = std::lower_bound(eventList.begin(), eventList.end(), t, less_EventEntry_double);
    return it==eventList.end() ? NULL : *it;
}

EventEntry *EventLog::getLastEventBefore(double t)
{
    // do getFirstEventAfter, then return the event before that
    EventEntryList::iterator it = std::lower_bound(eventList.begin(), eventList.end(), t, less_EventEntry_double);
    return it==eventList.begin() ? NULL : it==eventList.end() ? eventList.back() : *(it-1);
}

inline bool less_EventEntry_tableRowIndex(int index, EventEntry *e) {return index < e->tableRowIndex;}

EventEntry *EventLog::getEventByTableRowIndex(int tableRowIndex)
{
    //EventEntryList::iterator it = std::upper_bound(eventList.begin(), eventList.end(), tableRowIndex, less_EventEntry_tableRowIndex);
    //return it==eventList.end() ? NULL : *it;

    // FIXME replace this with some std::upper_bound() stuff like the one above (except that that one doesn't work)
    if (eventList.empty())
        return NULL;
    for (int i=0; i<eventList.size(); i++)
        if (eventList[i]->tableRowIndex > tableRowIndex)
            return i==0 ? NULL : eventList[i-1];
    return eventList.back();
}

void EventLog::deselectAllEvents()
{
    int n = eventList.size();
    for (int i=0; i<n; i++)
         eventList[i]->isSelected = false;
}

void EventLog::expandAllEvents()
{
    int n = eventList.size();
    int currentRow = 0;
    for (int i=0; i<n; i++)
    {
         eventList[i]->isExpandedInTree = true;
         eventList[i]->tableRowIndex = currentRow;
         currentRow += 1 + eventList[i]->numLogMessages;
    }
}

void EventLog::collapseAllEvents()
{
    int n = eventList.size();
    for (int i=0; i<n; i++)
    {
         eventList[i]->isExpandedInTree = false;
         eventList[i]->tableRowIndex = i;
    }
}

void EventLog::expandEvent(int pos)
{
    if (pos<0 || pos>=eventList.size())
        return; // index out of bounds
    if (eventList[pos]->isExpandedInTree)
        return; // already expanded

    // expand, and shift down all subsequent elements
    eventList[pos]->isExpandedInTree = true;
    int shiftby = eventList[pos]->numLogMessages;
    int n = eventList.size();
    for (int i=pos+1; i<n; i++)
         eventList[i]->tableRowIndex += shiftby;
}

void EventLog::collapseEvent(int pos)
{
    if (pos<0 || pos>=eventList.size())
        return; // index out of bounds
    if (!eventList[pos]->isExpandedInTree)
        return; // already collapsed

    // collapse, and shift up all subsequent elements
    eventList[pos]->isExpandedInTree = false;
    int shiftby = eventList[pos]->numLogMessages;
    int n = eventList.size();
    for (int i=pos+1; i<n; i++)
         eventList[i]->tableRowIndex -= shiftby;
}

char *EventLog::tokensToStr(int numTokens, char **vec)
{
    int length = numTokens;

    for (int i = 0; i < numTokens; i++)
        length += strlen(vec[i]);

    char *str = new char[length];
    length = 0;

    for (int i = 0; i < numTokens; i++)
    {
        strcpy(str + length, vec[i]);
        length += strlen(vec[i]);

        if (i != numTokens -1)
            *(str + length++) = ' ';
    }

    *(str + length) = '\0';

    return str;
}

inline bool less_EventEntryByEventNumber(EventEntry *e1, EventEntry *e2) {
    return e1->eventNumber < e2->eventNumber;
}
inline bool equal_EventEntryByEventNumber(EventEntry *e1, EventEntry *e2) {
    return e1->eventNumber == e2->eventNumber;
}
inline bool less_ModuleById(ModuleEntry *m1, ModuleEntry *m2) {
    return m1->moduleId < m2->moduleId;
}
inline bool less_MessageEntryByLineNumber(MessageEntry *m1, MessageEntry *m2) {
    return m1->lineNumber < m2->lineNumber;
}


EventLog *EventLog::traceEvent(EventEntry *tracedEvent, bool wantCauses, bool wantConsequences)
{
    EventLog *traceResult = new EventLog(this);

    std::vector<EventEntry*>& collectedEvents = traceResult->eventList;
    std::vector<MessageEntry*>& collectedMessages = traceResult->messageList;

    if (wantCauses)
    {
        for (EventEntryList::iterator it = eventList.begin(); it!=eventList.end(); ++it)
        {
            (*it)->isInCollectedEvents = false;
        }

        std::set<EventEntry*> openEvents;
        openEvents.insert(tracedEvent);
        tracedEvent->isInCollectedEvents = true;
        while (openEvents.size() > 0)
        {
            // remove one from openEvents, and add it into collectedEvents
            EventEntry *event = *openEvents.begin();
            openEvents.erase(openEvents.begin());
            collectedEvents.push_back(event);
            event->isInCollectedEvents = true;

            // then add all causes to openEvents
            for (MessageEntryList::iterator it = event->causes.begin(); it!=event->causes.end(); ++it)
            {
                MessageEntry *msg = *it;
                if (msg->source != NULL)
                {
                    collectedMessages.push_back(msg);
                    if (msg->source != event && !msg->source->isInCollectedEvents)
                        openEvents.insert(msg->source);
                }
            }
        }
    }

    if (wantConsequences)
    {
        for (EventEntryList::iterator it = eventList.begin(); it!=eventList.end(); ++it)
        {
            (*it)->isInCollectedEvents = false;
        }

        std::set<EventEntry*> openEvents;
        openEvents.insert(tracedEvent);
        tracedEvent->isInCollectedEvents = true;
        while (openEvents.size() > 0)
        {
            // remove one from openEvents, and add it into collectedEvents
            EventEntry *event = *openEvents.begin();
            openEvents.erase(openEvents.begin());
            collectedEvents.push_back(event);
            event->isInCollectedEvents = true;

            // then add all consequences to openEvents
            for (MessageEntryList::iterator it = event->consequences.begin(); it!=event->consequences.end(); ++it)
            {
                MessageEntry *msg = *it;
                if (msg->target != NULL)
                {
                    collectedMessages.push_back(msg);
                    if (msg->target != event && !msg->target->isInCollectedEvents)
                        openEvents.insert(msg->target);
                }
            }
        }
    }

    // sort events by event number (there cannot be duplicates here)
    sort(collectedEvents.begin(), collectedEvents.end(), less_EventEntryByEventNumber);

    // collect list of modules that occur in the filtered event list
    std::set<ModuleEntry*> moduleSet;
    for (int i=0; i<collectedEvents.size(); i++)
        moduleSet.insert(collectedEvents[i]->module);
    for (std::set<ModuleEntry*>::iterator it = moduleSet.begin(); it!=moduleSet.end(); ++it)
        traceResult->moduleList.push_back(*it);
    sort(traceResult->moduleList.begin(), traceResult->moduleList.end(), less_ModuleById);

    // sort messages by "source" event number (there cannot be duplicates here)
    sort(collectedMessages.begin(), collectedMessages.end(), less_MessageEntryByLineNumber);

    return traceResult;
}

void EventLog::writeTrace(FILE *fout)
{
    for (MessageEntryList::iterator it = messageList.begin(); it != messageList.end(); it++)
    {
        MessageEntry *messageEntry = *it;
        EventEntry *eventEntry = messageEntry->target;
        if (!eventEntry)
        {
            fprintf(stderr, "Ignoring wrong message at line %ld (no target event)\n", messageEntry->lineNumber); //XXX
            continue;
        }

        if (messageEntry->isDelivery)
            fprintf(fout, "*");
        else
            fprintf(fout, "+");

        // TODO: in some rare cases non delivery messages print the wrong module
        // FIXME: checked by diffing the original and printed log file

        fprintf(fout, " [%ld] %ld %.*g (%s) %s (id=%d) (%s) %s\n",
                eventEntry->eventNumber,
                messageEntry->source->eventNumber,
                12,
                eventEntry->simulationTime,
                eventEntry->module->moduleClassName,
                eventEntry->module->moduleFullPath.c_str(),
                eventEntry->module->moduleId,
                messageEntry->messageClassName,
                (messageEntry->messageName != NULL) ? messageEntry->messageName : "");

        for (std::vector<const char *>::iterator at = messageEntry->logMessages.begin(); at != messageEntry->logMessages.end(); at++)
            fprintf(fout, "  [%ld] %s\n", eventEntry->eventNumber, *at);
    }
}
