//=========================================================================
//  EVENTLOG.CC - part of
//                  OMNeT++/OMNEST
//           Discrete System Simulation in C++
//
//=========================================================================

/*--------------------------------------------------------------*
  Copyright (C) 1992-2006 Andras Varga

  This file is distributed WITHOUT ANY WARRANTY. See the file
  `license' for details on this and other legal matters.
*--------------------------------------------------------------*/

#include <algorithm>
#include <assert.h>
#include "eventlog.h"

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
        bool invalidFormat = false;
        int numTokens = ftok.numTokens();
        char **vec = ftok.tokens();
        lineNumber++;

        if (numTokens > 0)
        {
            if (!strcmp(vec[0], "*") || !strcmp(vec[0], "+"))
            {
                if (numTokens < 8)
                    invalidFormat = true;
                else
                {
                    bool isDelivery = !strcmp(vec[0], "*");   //FIXME stricter format check overall
                    // skip [ character
                    long eventNumber = atol(vec[1] + 1);
                    long causalEventNumber = !strcmp(vec[2], "initialize") ? -1 : atol(vec[2]);

                    messageEntry = new MessageEntry();
                    messageEntry->lineNumber = lineNumber;
                    messageEntry->isDelivery = isDelivery;

                    if (isDelivery)
                    {
                        eventEntry = new EventEntry();
                        eventEntry->eventNumber = eventNumber;
                        eventEntry->simulationTime = atof(vec[3]);
                        eventEntry->cause = messageEntry;
                        eventList.push_back(eventEntry);
                    }

                    // skip (id=
                    messageEntry->module = getOrAddModule(atoi(vec[6] + 4), vec[4], vec[5]);

                    if (causalEventNumber != -1)
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
                char *endPtr;
                char *eventNumberPtr = vec[0] + 1;
                long eventNumber = strtol(eventNumberPtr, &endPtr, 10);
                char *str = tokensToStr(numTokens - 1, vec + 1);

                if (endPtr != eventNumberPtr)
                {
                    if (messageEntry != NULL && messageEntry->target->eventNumber == eventNumber)
                        messageEntry->logMessages.push_back(stringPool.get(str));
                }
                else if (!strncmp("build", eventNumberPtr, 5))
                    buildLogMessages.push_back(stringPool.get(str));
                else if (!strncmp("initialize", eventNumberPtr, 10))
                    initializeLogMessages.push_back(stringPool.get(str));
                else if (!strncmp("finish", eventNumberPtr, 6))
                    finishLogMessages.push_back(stringPool.get(str));
                else
                    invalidFormat = true;

                delete [] str;
            }
        }
        else
            invalidFormat = true;

        if (invalidFormat)
        {
            // TODO: use returned line of readLine
            char *line = tokensToStr(numTokens, vec);
            fprintf(stderr, "Invalid format at line %ld: %s\n", lineNumber, line);
            delete [] line;
        }
    }

    // indexing: collect causes[] and consequences[] for each event; also calculate numLogMessages
    for (MessageEntryList::iterator it = messageList.begin(); it != messageList.end(); it++)
    {
        MessageEntry *messageEntry = *it;

        if (messageEntry->source != NULL)
            messageEntry->source->consequences.push_back(messageEntry);

        if (messageEntry->target != NULL) {
            messageEntry->target->causes.push_back(messageEntry);
            messageEntry->target->numLogMessages += messageEntry->logMessages.size();
        }
    }
}

int EventLog::getNumEvents()
{
    return eventList.size();
}

EventEntry *EventLog::getEvent(int pos)
{
    if (pos<0 || pos>=eventList.size() || !eventList[pos])
        throw new Exception("event index %d out of bounds", pos);
    return eventList[pos];
}

EventEntry *EventLog::getFirstEvent()
{
    return eventList.empty() ? NULL : eventList.front();
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
        throw new Exception("module index %d out of bounds", pos);
    return moduleList[pos];
}

int EventLog::getNumMessages()
{
    return messageList.size();
}

MessageEntry *EventLog::getMessage(int pos)
{
    if (pos<0 || pos>=messageList.size() || !messageList[pos])
        throw new Exception("message index %d out of bounds", pos);
    return messageList[pos];
}

std::vector<int> *EventLog::getMessagesIntersecting(long startEventNumber, long endEventNumber, std::set<int> *moduleIds, bool wantNonDeliveryMsgs)
{
    std::vector<int> *vp = new std::vector<int>;

    // Note: this is linear search, and could be optimized using some clever indexing if needed,
    // although this one is quite fast enough up to a few million messages
    int n = messageList.size();
    for (int i=0; i<n; i++)
    {
        MessageEntry *msg = messageList[i];
        if (wantNonDeliveryMsgs || msg->isDelivery)
        {
            EventEntry *src = msg->source;
            EventEntry *target = msg->target;

            if (src && target && src->eventNumber<endEventNumber && target->eventNumber>=startEventNumber)
            {
                if (!moduleIds || (moduleIds->find(msg->module->moduleId)!=moduleIds->end() &&
                                   moduleIds->find(src->cause->module->moduleId)!=moduleIds->end()))
                {
                    vp->push_back(i);
                }
            }
        }
    }
    return vp;
}

ModuleEntry *EventLog::getOrAddModule(int moduleId, char *moduleClassName, char *moduleFullPath)
{
    // skip () characters
    if (!strncmp(moduleClassName, "(", 1))
    {
        *(moduleClassName + strlen(moduleClassName) - 1) = '\0';
        moduleClassName++;
    }

    // if module with such ID already exists, return it
    //FIXME warn if className or fullPath doesn't match!
    for (ModuleEntryList::iterator it = moduleList.begin(); it != moduleList.end(); it++)
    {
        ModuleEntry *moduleEntry = *it;

        if (moduleEntry->moduleId == moduleId)
        {
            if (!strcmp(moduleEntry->moduleClassName, moduleClassName) &&
                !strcmp(moduleEntry->moduleFullPath.c_str(), moduleFullPath))
            {
                return *it;
            }
            else
            {
                printf("Different module data for the same module id, returning new module\n");
            }
        }
    }

    // not yet in there -- store it
    ModuleEntry *moduleEntry = new ModuleEntry();
    moduleEntry->moduleId = moduleId;
    moduleEntry->moduleClassName = stringPool.get(moduleClassName);
    moduleEntry->moduleFullPath = moduleFullPath;
    moduleList.push_back(moduleEntry);

    return moduleEntry;
}

inline bool less_EventEntry_EventNumber(EventEntry *e, long eventNumber) {return e->eventNumber < eventNumber;}

inline bool less_EventEntry_SimulationTime(EventEntry *e, double t) {return e->simulationTime < t;}

inline bool less_EventEntry_TimelineCoordinate(EventEntry *e, double t) {return e->timelineCoordinate < t;}

inline EventEntry *EventLog::getEventNULLSafe(int pos)
{
    if (pos == -1)
        return NULL;
    else
        return getEvent(pos);
}

inline int EventLog::getEventPositionByNumber(long eventNumber)
{
    EventEntryList::iterator it = std::lower_bound(eventList.begin(), eventList.end(), eventNumber, less_EventEntry_EventNumber);
    if (it==eventList.end() || ((EventEntry*)*it)->eventNumber != eventNumber)
        return -1;
    else
        return it - eventList.begin();
}

int EventLog::findEvent(EventEntry *event)
{
    int pos = getEventPositionByNumber(event->eventNumber);
    EventEntry *e = getEventNULLSafe(pos);
    return (e==event) ? pos : -1;
}

EventEntry *EventLog::getEventByNumber(long eventNumber)
{
    return getEventNULLSafe(getEventPositionByNumber(eventNumber));
}

inline int EventLog::getFirstEventPositionAfterByPredicate(double t, bool (*predicate)(EventEntry*,double))
{
    EventEntryList::iterator it = std::lower_bound(eventList.begin(), eventList.end(), t, predicate);

    if (it==eventList.end())
        return -1;
    else
        return it - eventList.begin();
}

inline int EventLog::getLastEventPositionBeforeByPredicate(double t, bool (*predicate)(EventEntry*,double))
{
    // do getFirstEventAfter, then return the event before that
    EventEntryList::iterator it = std::lower_bound(eventList.begin(), eventList.end(), t, predicate);

    if (it==eventList.end())
        return eventList.size() - 1;
    else if (predicate((EventEntry*)*it, t))
        return it - eventList.begin();
    else if (it==eventList.begin())
        return -1;
    else
        return (it - eventList.begin()) - 1;
}

EventEntry *EventLog::getFirstEventAfter(double t)
{
    return getEventNULLSafe(getFirstEventPositionAfter(t));
}

EventEntry *EventLog::getLastEventBefore(double t)
{
    return getEventNULLSafe(getLastEventPositionBefore(t));
}

int EventLog::getFirstEventPositionAfter(double t)
{
    return getFirstEventPositionAfterByPredicate(t, less_EventEntry_SimulationTime);
}

int EventLog::getLastEventPositionBefore(double t)
{
    return getLastEventPositionBeforeByPredicate(t, less_EventEntry_SimulationTime);
}

EventEntry *EventLog::getFirstEventAfterByTimelineCoordinate(double t)
{
    return getEventNULLSafe(getFirstEventPositionAfterByTimelineCoordinate(t));
}

EventEntry *EventLog::getLastEventBeforeByTimelineCoordinate(double t)
{
    return getEventNULLSafe(getLastEventPositionBeforeByTimelineCoordinate(t));
}

int EventLog::getFirstEventPositionAfterByTimelineCoordinate(double t)
{
    return getFirstEventPositionAfterByPredicate(t, less_EventEntry_TimelineCoordinate);
}

int EventLog::getLastEventPositionBeforeByTimelineCoordinate(double t)
{
    return getLastEventPositionBeforeByPredicate(t, less_EventEntry_TimelineCoordinate);
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

    char *str = new char[length + 1];
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
inline bool less_ModuleById(ModuleEntry *m1, ModuleEntry *m2) {
    return m1->moduleId < m2->moduleId;
}
inline bool less_MessageEntryByLineNumber(MessageEntry *m1, MessageEntry *m2) {
    return m1->lineNumber < m2->lineNumber;
}

EventLog *EventLog::traceEvent(EventEntry *tracedEvent, std::set<int> *moduleIds, bool wantCauses, bool wantConsequences, bool wantNonDeliveryMessages)
{
    EventLog *traceResult = new EventLog(this);

    std::vector<EventEntry*>& collectedEvents = traceResult->eventList;
    std::vector<MessageEntry*>& collectedMessages = traceResult->messageList;

    if (tracedEvent == NULL)
    {
        for (EventEntryList::iterator it = eventList.begin(); it!=eventList.end(); ++it)
            if (moduleIds == NULL || moduleIds->find((*it)->cause->module->moduleId) != moduleIds->end())
                collectedEvents.push_back(*it);

        for (MessageEntryList::iterator it = messageList.begin(); it!=messageList.end(); ++it)
            if ((wantNonDeliveryMessages || (*it)->isDelivery) &&
                (moduleIds == NULL || moduleIds->find((*it)->module->moduleId) != moduleIds->end()))
                collectedMessages.push_back(*it);
    }
    else
    {
        if (wantCauses)
        {
            for (EventEntryList::iterator it = eventList.begin(); it!=eventList.end(); ++it)
                (*it)->isInCollectedEvents = false;

            std::set<EventEntry*> openEvents;
            openEvents.insert(tracedEvent);
            tracedEvent->isInCollectedEvents = true;
            while (openEvents.size() > 0)
            {
                // remove one from openEvents, and add it into collectedEvents
                EventEntry *event = *openEvents.begin();
                openEvents.erase(openEvents.begin());
                event->isInCollectedEvents = true;
                if (moduleIds == NULL || moduleIds->find(event->cause->module->moduleId) != moduleIds->end())
                    collectedEvents.push_back(event);

                // then add all causes to openEvents
                for (MessageEntryList::iterator it = event->causes.begin(); it!=event->causes.end(); ++it)
                {
                    MessageEntry *msg = *it;
                    if (wantNonDeliveryMessages || msg->isDelivery)
                    {
                        if (moduleIds == NULL || moduleIds->find(msg->module->moduleId) != moduleIds->end())
                            collectedMessages.push_back(msg);
                        if (msg->source != NULL && msg->source != event && !msg->source->isInCollectedEvents)
                            openEvents.insert(msg->source);
                    }
                }
            }
        }

        if (wantConsequences)
        {
            for (EventEntryList::iterator it = eventList.begin(); it!=eventList.end(); ++it)
                (*it)->isInCollectedEvents = false;

            std::set<EventEntry*> openEvents;
            openEvents.insert(tracedEvent);
            tracedEvent->isInCollectedEvents = true;
            while (openEvents.size() > 0)
            {
                // remove one from openEvents, and add it into collectedEvents
                EventEntry *event = *openEvents.begin();
                openEvents.erase(openEvents.begin());
                event->isInCollectedEvents = true;
                // avoid duplicating tracedEvent already added during adding causes
                if (event != tracedEvent && (moduleIds == NULL || moduleIds->find(event->cause->module->moduleId) != moduleIds->end()))
                    collectedEvents.push_back(event);

                // then add all consequences to openEvents
                for (MessageEntryList::iterator it = event->consequences.begin(); it!=event->consequences.end(); ++it)
                {
                    MessageEntry *msg = *it;
                    if (wantNonDeliveryMessages || msg->isDelivery)
                    {
                        if (moduleIds == NULL || moduleIds->find(msg->module->moduleId) != moduleIds->end())
                            collectedMessages.push_back(msg);
                        if (msg->source != NULL && msg->target != event && !msg->target->isInCollectedEvents)
                            openEvents.insert(msg->target);
                    }
                }
            }
        }
    }

    // sort events by event number (there cannot be duplicates here)
    sort(collectedEvents.begin(), collectedEvents.end(), less_EventEntryByEventNumber);

    // collect list of modules that occur in the filtered event list
    std::set<ModuleEntry*> moduleSet;
    for (int i=0; i<collectedEvents.size(); i++)
        moduleSet.insert(collectedEvents[i]->cause->module);
    for (std::set<ModuleEntry*>::iterator it = moduleSet.begin(); it!=moduleSet.end(); ++it)
        traceResult->moduleList.push_back(*it);
    sort(traceResult->moduleList.begin(), traceResult->moduleList.end(), less_ModuleById);

    // sort messages by "source" event number (there cannot be duplicates here)
    sort(collectedMessages.begin(), collectedMessages.end(), less_MessageEntryByLineNumber);

    return traceResult;
}

/**
 * Returns a graph in a square matrix (number of nodes) counting the number of messages
 * going from one node to another. Nodes represent a number of modules given in a map.
 */
std::vector<int> *EventLog::buildMessageCountGraph(std::map<int, int> *moduleIdToNodeIdMap)
{
    std::set<int> nodeIdSet;

    for (std::map<int, int>::iterator it = moduleIdToNodeIdMap->begin(); it != moduleIdToNodeIdMap->end(); it++)
        nodeIdSet.insert(it->second);

    int numberOfNodes = nodeIdSet.size();
    std::vector<int> *result = new std::vector<int>();
    result->resize(numberOfNodes * numberOfNodes);

    for (MessageEntryList::iterator it = messageList.begin(); it != messageList.end(); it++)
    {
        MessageEntry *messageEntry = *it;
        EventEntry *source = messageEntry->source;
        EventEntry *target = messageEntry->target;

        if (source != NULL)
        {
            std::map<int, int>::iterator sourceModuleIdIt = moduleIdToNodeIdMap->find(source->cause->module->moduleId);
            std::map<int, int>::iterator targetModuleIdIt = moduleIdToNodeIdMap->find(target->cause->module->moduleId);

            if (sourceModuleIdIt !=  moduleIdToNodeIdMap->end() && targetModuleIdIt !=  moduleIdToNodeIdMap->end()) {
                result->at(sourceModuleIdIt->second * numberOfNodes + targetModuleIdIt->second)++;
            }
        }
    }

    return result;
}

void EventLog::writeTrace(FILE *fout)
{
    for (std::vector<const char *>::iterator it = buildLogMessages.begin(); it != buildLogMessages.end(); it++)
        fprintf(fout, "  [build] %s\n", *it);

    for (std::vector<const char *>::iterator it = initializeLogMessages.begin(); it != initializeLogMessages.end(); it++)
        fprintf(fout, "  [initialize] %s\n", *it);

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

        fprintf(fout, " [%ld]", eventEntry->eventNumber);

        if (messageEntry->source != NULL)
            fprintf(fout, " %ld", messageEntry->source->eventNumber);
        else
            fprintf(fout, " initialize");

        fprintf(fout, " %.*g (%s) %s (id=%d) (%s) %s\n",
                12,
                eventEntry->simulationTime,
                messageEntry->module->moduleClassName,
                messageEntry->module->moduleFullPath.c_str(),
                messageEntry->module->moduleId,
                messageEntry->messageClassName,
                (messageEntry->messageName != NULL) ? messageEntry->messageName : "");

        for (std::vector<const char *>::iterator at = messageEntry->logMessages.begin(); at != messageEntry->logMessages.end(); at++)
            fprintf(fout, "  [%ld] %s\n", eventEntry->eventNumber, *at);
    }

    for (std::vector<const char *>::iterator it = finishLogMessages.begin(); it != finishLogMessages.end(); it++)
        fprintf(fout, "  [finish] %s\n", *it);
}
