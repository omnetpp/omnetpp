//=========================================================================
//  EVENTLOGFACADE.CC - part of
//                  OMNeT++/OMNEST
//           Discrete System Simulation in C++
//
//=========================================================================

/*--------------------------------------------------------------*
  Copyright (C) 1992-2006 Andras Varga

  This file is distributed WITHOUT ANY WARRANTY. See the file
  `license' for details on this and other legal matters.
*--------------------------------------------------------------*/

#include "ievent.h"
#include "ieventlog.h"
#include "eventlogfacade.h"

EventLogFacade::EventLogFacade(IEventLog *eventLog)
{
    EASSERT(eventLog);
    this->eventLog = eventLog;
}

IEvent *EventLogFacade::getNthEventInDirection(IEvent *event, bool forward, long distance)
{
    while (distance > 0)
    {
        if (forward)
            event = event->getNextEvent();
        else
            event = event->getPreviousEvent();

        distance--;
    }

    return event;
}

void EventLogFacade::getEventLogEntryForEventLogTableRowIndex(long fixPointEventNumber, long fixPointLineNumber, long lineNumber, IEvent **event, EventLogEntry **eventLogEntry)
{
    long lineNumberDelta = lineNumber - fixPointLineNumber;
    bool forward = lineNumberDelta >= 0;

    IEvent *currentEvent;
    if (forward)
        currentEvent = eventLog->getEventForEventNumber(fixPointEventNumber);
    else
        currentEvent = eventLog->getEventForEventNumber(fixPointEventNumber)->getPreviousEvent();

    int currentNumLines = currentEvent != NULL ? currentEvent->getNumEventLogMessages() + 1 : 0;

    // search for event
    while (currentEvent &&
           ((forward && currentNumLines <= lineNumberDelta) ||
            (!forward && currentNumLines < -lineNumberDelta)))
    {
        if (forward)
        {
            currentEvent = currentEvent->getNextEvent();
            lineNumberDelta -= currentNumLines;
        }
        else
        {
            currentEvent = currentEvent->getPreviousEvent();
            lineNumberDelta += currentNumLines;
        }

        currentNumLines = currentEvent != NULL ? currentEvent->getNumEventLogMessages() + 1 : 0;
    }

    // get message line
    EventLogEntry *currentEventLogEntry;

    if (!currentEvent)
        currentEventLogEntry = NULL;
    else if (lineNumberDelta == 0)
        currentEventLogEntry = currentEvent->getEventEntry();
    else if (lineNumberDelta > 0)
        currentEventLogEntry = currentEvent->getEventLogMessage(lineNumberDelta - 1);
    else
        currentEventLogEntry = currentEvent->getEventLogMessage(currentEvent->getNumEventLogMessages() + lineNumberDelta);

    *event = currentEvent;
    *eventLogEntry = currentEventLogEntry;
}

IEvent *EventLogFacade::getEventForEventLogTableRowIndex(long fixPointEventNumber, long fixPointLineNumber, long lineNumber)
{
    IEvent *event;
    EventLogEntry *eventLogEntry;
    getEventLogEntryForEventLogTableRowIndex(fixPointEventNumber, fixPointLineNumber, lineNumber, &event, &eventLogEntry);
    return event;
}

EventLogEntry *EventLogFacade::getEventLogEntryForEventLogTableRowIndex(long fixPointEventNumber, long fixPointLineNumber, long lineNumber)
{
    IEvent *event;
    EventLogEntry *eventLogEntry;
    getEventLogEntryForEventLogTableRowIndex(fixPointEventNumber, fixPointLineNumber, lineNumber, &event, &eventLogEntry);
    return eventLogEntry;
}
