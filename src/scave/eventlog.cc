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
}

EventEntry::~EventEntry()
{
}

/*--------------------------------------------------------------*/
MessageEntry::MessageEntry()
{
    delivery = false;
    lineNumber = -1;
}

MessageEntry::~MessageEntry()
{
}

/*--------------------------------------------------------------*/
EventLog::EventLog()
{
    logFileName = NULL;
    tracedEvent = NULL;
}

EventLog::EventLog(const char *logFileName)
{
    this->logFileName = logFileName;
    tracedEvent = NULL;

    parseLogFile();
}

EventLog::~EventLog()
{
    for (ModuleEntryList::iterator it = moduleList.begin(); it != moduleList.end(); it++)
        delete *it;

    for (EventEntryList::iterator it = eventList.begin(); it != eventList.end(); it++)
        delete *it;

    for (MessageEntryList::iterator it = messageList.begin(); it != messageList.end(); it++)
        delete *it;
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
                    fprintf(stderr, "Invalid format at line %ld: %s\n", lineNumber, tokensToStr(numTokens, vec));
                }
                else
                {
                    bool delivery = !strcmp(vec[0], "*");   //FIXME stricter format check overall
                    // skip [ character
                    long eventNumber = atol(vec[1] + 1);
                    long causalEventNumber = atol(vec[2]);

                    messageEntry = new MessageEntry();
                    messageEntry->lineNumber = lineNumber;
                    messageEntry->delivery = delivery;

                    if (delivery)
                    {
                        EventEntry *eventEntry = new EventEntry();
                        eventEntry->eventNumber = eventNumber;
                        eventEntry->simulationTime = atof(vec[3]);
                        // skip (id=
                        eventEntry->module = getModule(atoi(vec[6] + 4), vec[4], vec[5]);
                        eventEntry->cause = messageEntry;
                        eventList.push_back(eventEntry);
                    }

                    messageEntry->source = getEvent(causalEventNumber);
                    messageEntry->target = getEvent(eventNumber);
                    // skip () characters
                    *(vec[7] + strlen(vec[7]) - 1) = '\0';
                    messageEntry->messageClassName = vec[7] + 1;

                    if (numTokens == 9)
                        messageEntry->messageName = vec[8];

                    messageList.push_back(messageEntry);
                }
            }
            else
            {
                if (messageEntry != NULL)
                {
                    char *str = tokensToStr(numTokens - 1, vec + 1);
                    messageEntry->logMessages.push_back(str);
                    delete [] str;
                }
            }
        }
    }

    for (MessageEntryList::iterator it = messageList.begin(); it != messageList.end(); it++)
    {
        MessageEntry *messageEntry = *it;
        messageEntry->source->consequences.push_back(messageEntry);
        messageEntry->target->causes.push_back(messageEntry);
    }
}

long EventLog::getNumEvents()
{
    return eventList.size();
}

EventEntry *EventLog::getEvent(long eventNumber)
{
    if (eventNumber<0 || eventNumber>=eventList.size() || !eventList[eventNumber])
    {
        fprintf(stderr, "Could not find event for: %ld", eventNumber);
        return NULL;
    }

    return eventList[eventNumber];
}

ModuleEntry *EventLog::getModule(int moduleId, char *moduleClassName, char *moduleFullName)
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
    moduleEntry->moduleClassName = moduleClassName + 1;
    moduleEntry->moduleFullName = moduleFullName;
    moduleList.push_back(moduleEntry);

    return moduleEntry;
}

inline bool less_EventEntry_double(EventEntry *e, double t) {return e->simulationTime < t;}

EventEntry *EventLog::getFirstEventAfter(double t)
{
    EventEntryList::iterator it = std::lower_bound(eventList.begin(), eventList.end(), t, less_EventEntry_double);
    return it==eventList.end() ? NULL : *it;
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

EventLog *EventLog::traceEvent(long tracedEventNumber, bool causes, bool consequences)
{
    EventLog *traceResult = new EventLog();
/*
    traceResult.clear();
    tracedEvent = eventMap[tracedEventNumber];

    if (eventMap[tracedEventNumber] == NULL)
    {
        fprintf(stderr, "Event number %ld is not present in log file %s\n", tracedEventNumber, logFileName);
        return;
    }

    std::list<long> openEventNumbers;
    std::vector<long> collectedEventNumbers;
    openEventNumbers.push_back(tracedEventNumber);

    while (openEventNumbers.size() > 0)
    {
        long eventNumber = openEventNumbers.front();
        collectedEventNumbers.push_back(eventNumber);
        openEventNumbers.pop_back();
        EventEntry *entry = eventMap[eventNumber];

        for (std::list<long>::iterator it = entry->causalEventNumbers.begin();
            it != entry->causalEventNumbers.end();
            it++)
        {
            long causalEventNumber = *it;

            if (eventNumber != causalEventNumber)
                openEventNumbers.push_back(causalEventNumber);
        }
    }

    sort(collectedEventNumbers.begin(), collectedEventNumbers.end());

    for (std::vector<long>::iterator it = collectedEventNumbers.begin();
        it != collectedEventNumbers.end();
        it++)
    {
        traceResult.push_back(eventMap[*it]);
    }
*/
    return traceResult;
}

void EventLog::writeTrace(FILE *fout)
{
    for (MessageEntryList::iterator it = messageList.begin(); it != messageList.end(); it++)
    {
        MessageEntry *messageEntry = *it;
        EventEntry *eventEntry = messageEntry->target;

        if (messageEntry->delivery)
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
                eventEntry->module->moduleClassName.c_str(),
                eventEntry->module->moduleFullName.c_str(),
                eventEntry->module->moduleId,
                messageEntry->messageClassName.c_str(),
                messageEntry->messageName.c_str());

        for (std::vector<std::string>::iterator at = messageEntry->logMessages.begin(); at != messageEntry->logMessages.end(); at++)
        {
            std::string logMessage = *at;
            fprintf(fout, "  [%ld] %s\n", eventEntry->eventNumber, logMessage.c_str());
        }
    }
}
