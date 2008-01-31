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

USING_NAMESPACE

/**************************************************/

IMessageDependency::IMessageDependency(IEventLog *eventLog, bool isReuse)
{
    this->eventLog = eventLog;
    this->isReuse = isReuse;
}

/**************************************************/

MessageDependency::MessageDependency(IEventLog *eventLog, bool isReuse, long eventNumber, int beginSendEntryNumber)
    : IMessageDependency(eventLog, isReuse)
{
    Assert(eventNumber >= 0 && beginSendEntryNumber >= 0);

    if (isReuse) {
        this->causeEventNumber = EVENT_NOT_YET_CALCULATED;
        this->causeBeginSendEntryNumber = -1;
        this->consequenceEventNumber = eventNumber;
        this->consequenceBeginSendEntryNumber = beginSendEntryNumber;
    }
    else {
        this->causeEventNumber = eventNumber;
        this->causeBeginSendEntryNumber = beginSendEntryNumber;
        this->consequenceEventNumber = EVENT_NOT_YET_CALCULATED;
        this->consequenceBeginSendEntryNumber = -1;
    }
}

MessageDependency *MessageDependency::duplicate(IEventLog *eventLog)
{
    MessageDependency *messageDependency = new MessageDependency(*this);
    messageDependency->eventLog = eventLog;
    return messageDependency;
}

BeginSendEntry *MessageDependency::getCauseBeginSendEntry()
{
    Assert(causeBeginSendEntryNumber != -1);
    IEvent *event = getCauseEvent();
    Assert(event);
    return (BeginSendEntry *)event->getEventLogEntry(causeBeginSendEntryNumber);
}

long MessageDependency::getCauseEventNumber()
{
    if (causeEventNumber == EVENT_NOT_YET_CALCULATED)
    {
        // only consequence is present, calculate cause from it
        IEvent *consequenceEvent = getConsequenceEvent();
        Assert(consequenceEvent);
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

simtime_t& MessageDependency::getCauseSimulationTime()
{
    return getCauseEvent()->getSimulationTime();
}

BeginSendEntry *MessageDependency::getConsequenceBeginSendEntry()
{
    Assert(consequenceBeginSendEntryNumber != -1);
    IEvent *event = getConsequenceEvent();
    Assert(event);
    return (BeginSendEntry *)event->getEventLogEntry(consequenceBeginSendEntryNumber);
}

long MessageDependency::getConsequenceEventNumber()
{
    if (consequenceEventNumber == EVENT_NOT_YET_CALCULATED || consequenceEventNumber == EVENT_NOT_YET_REACHED)
    {
        // only cause is present, calculate consequence from it.
        //
        // when a message is scheduled/sent, we don't know the arrival event number
        // yet, only the simulation time is recorded in the event log file.
        // So here we have to look through all events at the arrival time,
        // and find the one "caused by" our message.
        simtime_t consequenceTime = getConsequenceSimulationTime();

        if (consequenceTime == simtime_nil)
            consequenceEventNumber = NO_SUCH_EVENT;
        else {
            IEvent *event = eventLog->getEventForSimulationTime(consequenceTime, FIRST_OR_PREVIOUS);

            // TODO: LONG RUNNING OPERATION
            while (event)
            {
                if (!event)
                {
                    // end of file
                    consequenceEventNumber = EVENT_NOT_YET_REACHED;
                    break;
                }

                if (event->getCauseEventNumber() == getCauseEventNumber() &&
                    event->getMessageId() == getCauseMessageId())
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

simtime_t& MessageDependency::getConsequenceSimulationTime()
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
            EventLogEntry *eventLogEntry = event->getEventLogEntry(i++);

            EndSendEntry *endSendEntry = dynamic_cast<EndSendEntry *>(eventLogEntry);
            if (endSendEntry)
                return endSendEntry->arrivalTime;

            DeleteMessageEntry *deleteMessageEntry = dynamic_cast<DeleteMessageEntry *>(eventLogEntry);
            if (deleteMessageEntry) {
                Assert(deleteMessageEntry->messageId == beginSendEntry->messageId);
                return simtime_nil;
            }
        }

        throw opp_runtime_error("Missing end message send entry");
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

FilteredMessageDependency::FilteredMessageDependency(IEventLog *eventLog, bool isReuse, IMessageDependency *beginMessageDependency, IMessageDependency *endMessageDependency)
    : IMessageDependency(eventLog, isReuse)
{
    this->beginMessageDependency = beginMessageDependency;
    this->endMessageDependency = endMessageDependency;
}

FilteredMessageDependency::~FilteredMessageDependency()
{
    delete beginMessageDependency;
    delete endMessageDependency;
}

FilteredMessageDependency *FilteredMessageDependency::duplicate(IEventLog *eventLog)
{
    return new FilteredMessageDependency(eventLog, isReuse, beginMessageDependency->duplicate(eventLog), endMessageDependency->duplicate(eventLog));
}

IEvent *FilteredMessageDependency::getCauseEvent()
{
    long causeEventNumber = beginMessageDependency->getCauseEventNumber();

    if (causeEventNumber < 0)
        return NULL;
    else
        return eventLog->getEventForEventNumber(causeEventNumber);
}

IEvent *FilteredMessageDependency::getConsequenceEvent()
{
    long consequenceEventNumber = endMessageDependency->getConsequenceEventNumber();

    if (consequenceEventNumber < 0)
        return NULL;
    else
        return eventLog->getEventForEventNumber(consequenceEventNumber);
}

void FilteredMessageDependency::print(FILE *file)
{
    beginMessageDependency->print(file);
    endMessageDependency->print(file);
}
