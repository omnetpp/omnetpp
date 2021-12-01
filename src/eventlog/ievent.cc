//=========================================================================
//  IEVENTLOG.CC - part of
//                  OMNeT++/OMNEST
//           Discrete System Simulation in C++
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
    cachedTimelineCoordinateBegin = -1;
    cachedTimelineCoordinateEnd = -1;
    cachedTimelineCoordinateSystemVersion = -1;
}

void IEvent::synchronize(FileReader::FileChange change)
{
    if (change != FileReader::UNCHANGED) {
        switch (change) {
            case FileReader::OVERWRITTEN:
                clearInternalState();
                break;
            case FileReader::APPENDED:
                break;
            default:
                throw opp_runtime_error("Unknown file change");
        }
    }
}

int IEvent::findBeginSendEntryIndex(int messageId)
{
    // find the "BS" or "SA" line
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

}  // namespace eventlog
}  // namespace omnetpp

