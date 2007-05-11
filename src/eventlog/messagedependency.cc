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

/**************************************************/

IMessageDependency::IMessageDependency(IEventLog *eventLog)
{
    this->eventLog = eventLog;
}

/**************************************************/

MessageDependency::MessageDependency(IEventLog *eventLog,
                                     long causeEventNumber, int causeBeginSendEntryNumber,
                                     long consequenceEventNumber, int consequenceBeginSendEntryNumber)
    : IMessageDependency(eventLog)
{
    this->causeEventNumber = causeEventNumber;
    this->consequenceEventNumber = consequenceEventNumber;
    this->causeBeginSendEntryNumber = causeBeginSendEntryNumber;
    this->consequenceBeginSendEntryNumber = consequenceBeginSendEntryNumber;

    Assert(causeEventNumber >= 0 || consequenceEventNumber >= 0);
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

simtime_t MessageDependency::getCauseSimulationTime()
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
    if (consequenceEventNumber == EVENT_NOT_YET_CALCULATED)
    {
        // only cause is present, calculate consequence from it.
        //
        // when a message is scheduled/sent, we don't know the arrival event number
        // yet, only the simulation time is recorded in the event log file.
        // So here we have to look through all events at the arrival time,
        // and find the one "caused by" our message.
        simtime_t consequenceTime = getConsequenceSimulationTime();

        if (consequenceTime == -1)
            consequenceEventNumber = NO_SUCH_EVENT;
        else {
            IEvent *event = eventLog->getEventForSimulationTime(consequenceTime, FIRST_OR_PREVIOUS);

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

simtime_t MessageDependency::getConsequenceSimulationTime()
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
                return -1;
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

MessageReuse::MessageReuse(IEventLog *eventLog, long senderEventNumber, int beginSendEnrtyNumber)
    : MessageDependency(eventLog, EVENT_NOT_YET_CALCULATED, -1, senderEventNumber, beginSendEnrtyNumber)
{
}

MessageReuse *MessageReuse::duplicate(IEventLog *eventLog)
{
    MessageReuse *messageReuse = new MessageReuse(*this);
    messageReuse->eventLog = eventLog;
    return messageReuse;
}

/**************************************************/

MessageSend::MessageSend(IEventLog *eventLog, long senderEventNumber, int beginSendEntryNumber)
    : MessageDependency(eventLog, senderEventNumber, beginSendEntryNumber, EVENT_NOT_YET_CALCULATED, -1)
{
}

MessageSend *MessageSend::duplicate(IEventLog *eventLog)
{
    MessageSend *messageSend = new MessageSend(*this);
    messageSend->eventLog = eventLog;
    return messageSend;
}

/**************************************************/

FilteredMessageDependency::FilteredMessageDependency(IEventLog *eventLog, IMessageDependency *beginMessageDependency, IMessageDependency *endMessageDependency)
    : IMessageDependency(eventLog)
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
    return new FilteredMessageDependency(eventLog, beginMessageDependency->duplicate(eventLog), endMessageDependency->duplicate(eventLog));
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
