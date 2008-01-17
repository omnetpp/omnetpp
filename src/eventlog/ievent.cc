//=========================================================================
//  IEVENTLOG.CC - part of
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

USING_NAMESPACE

IEvent::IEvent()
{
    nextEvent = NULL;
    previousEvent = NULL;
    isExpandedInEventLogTable = true;
    cachedTimelineCoordinate = -1;
    cachedTimelineCoordinateSystemVersion = -1;
}

int IEvent::findBeginSendEntryIndex(int messageId)
{
    // find the "BS" or "SA" line in the cause event
    for (int beginSendEntryNumber = 0; beginSendEntryNumber < getNumEventLogEntries(); beginSendEntryNumber++)
    {
        BeginSendEntry *beginSendEntry = dynamic_cast<BeginSendEntry *>(getEventLogEntry(beginSendEntryNumber));

        if (beginSendEntry && beginSendEntry->messageId == messageId)
            return beginSendEntryNumber;
    }

    return -1;
}

void IEvent::linkEvents(IEvent *previousEvent, IEvent *nextEvent)
{
    // used to build the linked list
    previousEvent->nextEvent = nextEvent;
    nextEvent->previousEvent = previousEvent;
}

void IEvent::unlinkEvents(IEvent *previousEvent, IEvent *nextEvent)
{
    // used to build the linked list
    previousEvent->nextEvent = NULL;
    nextEvent->previousEvent = NULL;
}
