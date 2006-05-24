//=========================================================================
//  EVENTTRACE.CC - part of
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
    //XXX test if this one is faster:
    //while (pool.size()>0) {
    //    delete [] *pool.begin();
    //    pool.erase(pool.begin());
    //}
}

const char *StringPool::get(const char *s)
{
    if (s==NULL)
        return NULL;
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
    this->logFileName = logFileName;
    tracedEvent = NULL;

    long lineNumber = 0;
    MessageEntry *messageEntry = NULL;
    FileTokenizer ftok(logFileName);

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
                        EventEntry *eventEntry = new EventEntry();
                        eventEntry->eventNumber = eventNumber;
                        eventEntry->simulationTime = atof(vec[3]);
                        // skip (id=
                        eventEntry->module = getOrAddModule(atoi(vec[6] + 4), vec[4], vec[5]);
                        eventEntry->cause = messageEntry;
                        eventList.push_back(eventEntry);
                    }

                    messageEntry->source = getEvent(causalEventNumber);
                    messageEntry->target = getEvent(eventNumber);
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
                if (messageEntry != NULL)
                {
                    char *str = tokensToStr(numTokens - 1, vec + 1);
                    messageEntry->logMessages.push_back(stringPool.get(str));
                    delete [] str;
                }
            }
        }
    }

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

ModuleEntry *EventLog::getOrAddModule(int moduleId, char *moduleClassName, char *moduleFullPath)
{
    // if module with such ID already exists, return it
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

EventLog *EventLog::traceEvent(EventEntry *tracedEvent, bool wantCauses, bool wantConsequences)
{
    EventLog *traceResult = new EventLog(this);

    std::vector<EventEntry*>& collectedEvents = traceResult->eventList;

    if (wantCauses)
    {
        std::set<EventEntry*> openEvents;
        openEvents.insert(tracedEvent);
        while (openEvents.size() > 0)
        {
            // remove one from openEvents, and add it into collectedEvents
            EventEntry *event = *openEvents.begin();
            openEvents.erase(openEvents.begin());
            collectedEvents.push_back(event);

            // then add all causes to openEvents
            for (MessageEntryList::iterator it = event->causes.begin(); it!=event->causes.end(); ++it)
                if ((*it)->source != event)
                    openEvents.insert((*it)->source);
        }
    }

    if (wantConsequences)
    {
        std::set<EventEntry*> openEvents;
        openEvents.insert(tracedEvent);
        while (openEvents.size() > 0)
        {
            // remove one from openEvents, and add it into collectedEvents
            EventEntry *event = *openEvents.begin();
            openEvents.erase(openEvents.begin());
            collectedEvents.push_back(event);

            // then add all consequences to openEvents
            for (MessageEntryList::iterator it = event->consequences.begin(); it!=event->consequences.end(); ++it)
                if ((*it)->target != event)
                    openEvents.insert((*it)->target);
        }
    }

    // sort events by event number, and filter out duplicates (if an event is reached
    // via several different branches, it'll be a duplicate)
    sort(collectedEvents.begin(), collectedEvents.end(), less_EventEntryByEventNumber);
    EventEntryList::iterator newEnd = unique(collectedEvents.begin(), collectedEvents.end(), equal_EventEntryByEventNumber);
    collectedEvents.resize(newEnd - collectedEvents.begin());

    // collect list of modules that occur in the filtered event list
    std::set<ModuleEntry*> moduleSet;
    for (int i=0; i<collectedEvents.size(); i++)
        moduleSet.insert(collectedEvents[i]->module);
    for (std::set<ModuleEntry*>::iterator it = moduleSet.begin(); it!=moduleSet.end(); ++it)
        traceResult->moduleList.push_back(*it);
    sort(traceResult->moduleList.begin(), traceResult->moduleList.end(), less_ModuleById);

    return traceResult;
}

void EventLog::writeTrace(FILE *fout)
{
    //FIXME this won't work with filtered eventlogs, where messageList is empty
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
                messageEntry->messageName);

        for (std::vector<const char *>::iterator at = messageEntry->logMessages.begin(); at != messageEntry->logMessages.end(); at++)
            fprintf(fout, "  [%ld] %s\n", eventEntry->eventNumber, *at);
    }
}
