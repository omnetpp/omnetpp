//=========================================================================
//  IEVENTLOG.CC - part of
//                  OMNeT++/OMNEST
//           Discrete System Simulation in C++
//
//  Author: Levente Meszaros
//
//=========================================================================

/*--------------------------------------------------------------*
  Copyright (C) 2006-2017 OpenSim Ltd.

  This file is distributed WITHOUT ANY WARRANTY. See the file
  `license' for details on this and other legal matters.
*--------------------------------------------------------------*/

#include <cstdio>
#include "ievent.h"

namespace omnetpp {
namespace eventlog {

IEvent::IEvent()
{
    clearInternalState();
}

void IEvent::clearInternalState()
{
    nextEvent = nullptr;
    previousEvent = nullptr;
    cachedTimelineCoordinate = -1;
    cachedTimelineCoordinateSystemVersion = -1;
}

void IEvent::synchronize(FileReader::FileChangedState change)
{
    if (change == FileReader::OVERWRITTEN)
        clearInternalState();
}

int IEvent::findBeginSendEntryIndex(int messageId)
{
    // find the "BS" or "SA" line in the cause event
    for (int beginSendEntryNumber = 0; beginSendEntryNumber < getNumEventLogEntries(); beginSendEntryNumber++) {
        BeginSendEntry *beginSendEntry = dynamic_cast<BeginSendEntry *>(getEventLogEntry(beginSendEntryNumber));

        if (beginSendEntry && beginSendEntry->messageId == messageId)
            return beginSendEntryNumber;
    }

    return -1;
}

void IEvent::linkEvents(IEvent *previousEvent, IEvent *nextEvent)
{
    previousEvent->nextEvent = nextEvent;
    nextEvent->previousEvent = previousEvent;
}

void IEvent::unlinkEvents(IEvent *previousEvent, IEvent *nextEvent)
{
    previousEvent->nextEvent = nullptr;
    nextEvent->previousEvent = nullptr;
}

void IEvent::unlinkNeighbourEvents(IEvent *event)
{
    if (event->previousEvent)
        unlinkEvents(event->previousEvent, event);
    if (event->nextEvent)
        unlinkEvents(event, event->nextEvent);
}

} // namespace eventlog
}  // namespace omnetpp

