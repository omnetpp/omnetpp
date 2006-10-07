//=========================================================================
//  MESSAGEDEPENDENCY.CC - part of
//                  OMNeT++/OMNEST
//           Discrete System Simulation in C++
//
//=========================================================================

/*--------------------------------------------------------------*
  Copyright (C) 1992-2006 Andras Varga

  This file is distributed WITHOUT ANY WARRANTY. See the file
  `license' for details on this and other legal matters.
*--------------------------------------------------------------*/

#include "eventlogdefs.h"
#include "ievent.h"
#include "ieventlog.h"
#include "eventlogentry.h"
#include "messagedependency.h"

MessageDependency::MessageDependency(IEventLog *eventLog,
    long causeEventNumber, int causeBeginSendEntryNumber,
    long consequenceEventNumber, int consequenceBeginSendEntryNumber)
{
    this->eventLog = eventLog;
    this->causeEventNumber = causeEventNumber;
    this->consequenceEventNumber = consequenceEventNumber;
    this->causeBeginSendEntryNumber = causeBeginSendEntryNumber;
    this->consequenceBeginSendEntryNumber = consequenceBeginSendEntryNumber;

    EASSERT(causeEventNumber >= 0 || consequenceEventNumber >= 0);
}

BeginSendEntry *MessageDependency::getBeginSendEntry()
{
    if (causeBeginSendEntryNumber != -1 && consequenceBeginSendEntryNumber != -1)
        return NULL;
    else if (causeBeginSendEntryNumber != -1)
        return getCauseBeginSendEntry();
    else
        return getConsequenceBeginSendEntry();
}

BeginSendEntry *MessageDependency::getCauseBeginSendEntry()
{
    return (BeginSendEntry *)getCauseEvent()->getEventLogEntry(causeBeginSendEntryNumber);
}

long MessageDependency::getCauseEventNumber()
{
    if (causeEventNumber == EVENT_NOT_YET_CALCULATED)
    {
        // only consequence is present, calculate cause from it
        IEvent *consequenceEvent = getConsequenceEvent();
        EASSERT(consequenceEvent);
        BeginSendEntry *beginSendEntry = (BeginSendEntry *)consequenceEvent->getEventLogEntry(consequenceBeginSendEntryNumber);
        causeEventNumber = beginSendEntry->previousEventNumber;
    }

    return causeEventNumber;
}

IEvent *MessageDependency::getCauseEvent()
{
    long causeEventNumber = getCauseEventNumber();

    if (causeEventNumber < 0)
        return NULL;
    else
        return eventLog->getEventForEventNumber(causeEventNumber);
}

simtime_t MessageDependency::getCauseTime()
{
    return getCauseEvent()->getSimulationTime();
}

BeginSendEntry *MessageDependency::getConsequenceBeginSendEntry()
{
    return (BeginSendEntry *)getConsequenceEvent()->getEventLogEntry(consequenceBeginSendEntryNumber);
}

long MessageDependency::getConsequenceEventNumber()
{
    if (consequenceEventNumber == EVENT_NOT_YET_CALCULATED)
    {
        // only cause is present, calculate consequence from it.
        //
        // when a message is scheduled/sent, we don't know the arrival event number
        // yet, only the simulation time is recorded in the event log file.
        // So here we have to look through all events at the arrival time,
        // and find the one "caused by" our message.
        simtime_t consequenceTime = getConsequenceTime();
        IEvent *event = eventLog->getEventForSimulationTime(consequenceTime, FIRST);

        while (event)
        {
            if (event == NULL)
            {
                // end of file
                consequenceEventNumber = EVENT_NOT_YET_REACHED;
                break;
            }

            if (event->getCauseEventNumber() == getCauseEventNumber())
            {
                consequenceEventNumber = event->getEventNumber();
                break;
            }

            if (event->getSimulationTime() > consequenceTime)
            {
                // no more event at that simulation time, and consequence event
                // still not found. It must have been cancelled (self message),
                // or it is not in the file (filtered out by the user, etc).
                consequenceEventNumber = NO_SUCH_EVENT;
                break;
            }

            event = event->getNextEvent();
        }
    }

    return consequenceEventNumber;
}

IEvent *MessageDependency::getConsequenceEvent()
{
    long consequenceEventNumber = getConsequenceEventNumber();

    if (consequenceEventNumber < 0)
        return NULL;
    else
        return eventLog->getEventForEventNumber(consequenceEventNumber);
}

simtime_t MessageDependency::getConsequenceTime()
{
    if (consequenceEventNumber >= 0)
        return getConsequenceEvent()->getSimulationTime();
    else
    {
        // find the arrival time of the message
        IEvent *event = getCauseEvent();
        BeginSendEntry *beginSendEntry = (BeginSendEntry *)(event->getEventLogEntry(causeBeginSendEntryNumber));

        // arrival time is in the EndSendEntry ("ES" line), find it
        int i = causeBeginSendEntryNumber + 1;

        while (i < event->getNumEventLogEntries())
        {
            EndSendEntry *endSendEntry = dynamic_cast<EndSendEntry *>(event->getEventLogEntry(i++));

            if (endSendEntry != NULL)
                return endSendEntry->arrivalTime;
        }

        throw new Exception("Missing end message send entry");
    }
}

void MessageDependency::print(FILE *file)
{
    printCause(file);
    printConsequence(file);
}

void MessageDependency::printCause(FILE *file)
{
    if (getCauseEventNumber() >= 0)
        getCauseEvent()->getEventEntry()->print(file);

    if (getCauseBeginSendEntryNumber() != -1)
        getCauseBeginSendEntry()->print(file);
}

void MessageDependency::printConsequence(FILE *file)
{
    if (getConsequenceEventNumber() >= 0)
       getConsequenceEvent()->getEventEntry()->print(file);

    if (getConsequenceBeginSendEntryNumber() != -1)
       getConsequenceBeginSendEntry()->print(file);
}

/**************************************************/

MessageReuse::MessageReuse(IEventLog *eventLog, long senderEventNumber, int BeginSendEntryNumber)
    : MessageDependency(eventLog, EVENT_NOT_YET_CALCULATED, -1, senderEventNumber, BeginSendEntryNumber)
{
}

/**************************************************/

MessageSend::MessageSend(IEventLog *eventLog, long senderEventNumber, int BeginSendEntryNumber)
    : MessageDependency(eventLog, senderEventNumber, BeginSendEntryNumber, EVENT_NOT_YET_CALCULATED, -1)
{
}

/**************************************************/
FilteredMessageDependency::FilteredMessageDependency(IEventLog *eventLog,
    long causeEventNumber, int causeBeginSendEntryNumber,
    long middleEventNumber, int middleBeginSendEntryNumber,
    long consequenceEventNumber, int consequenceBeginSendEntryNumber)
    : MessageDependency(eventLog, causeEventNumber, causeBeginSendEntryNumber, consequenceEventNumber, consequenceBeginSendEntryNumber)
{
    this->middleEventNumber = middleEventNumber;
    this->middleBeginSendEntryNumber = middleBeginSendEntryNumber;
}

IEvent *FilteredMessageDependency::getMiddleEvent()
{
    return eventLog->getEventForEventNumber(middleEventNumber);
}

simtime_t FilteredMessageDependency::getMiddleTime()
{
    return getMiddleEvent()->getSimulationTime();
}

BeginSendEntry *FilteredMessageDependency::getMiddleBeginSendEntry()
{
    return (BeginSendEntry *)getMiddleEvent()->getEventLogEntry(middleBeginSendEntryNumber);
}

void FilteredMessageDependency::printMiddle(FILE *file)
{
    if (getMiddleEventNumber() >= 0)
       getMiddleEvent()->getEventEntry()->print(file);

    if (getMiddleBeginSendEntryNumber() != -1)
       getMiddleBeginSendEntry()->print(file);
}

void FilteredMessageDependency::print(FILE *file)
{
    printCause(file);
    printMiddle(file);
    printConsequence(file);
}