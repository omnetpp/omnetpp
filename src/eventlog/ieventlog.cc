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
#include <cstdlib>
#include "ieventlog.h"

namespace omnetpp {
namespace eventlog {

IEventLog::IEventLog()
{
    clearInternalState();
}

void IEventLog::clearInternalState()
{
    lastNeighbourEventNumber = -1;
    lastNeighbourEvent = nullptr;
}

void IEventLog::synchronize(FileReader::FileChangedState change)
{
    if (change != FileReader::UNCHANGED)
        clearInternalState();
}

IEvent *IEventLog::getNeighbourEvent(IEvent *event, eventnumber_t distance)
{
    Assert(event);
    eventnumber_t neighbourEventNumber = event->getEventNumber() + distance;

    if (lastNeighbourEvent && lastNeighbourEventNumber != -1 && abs64(neighbourEventNumber - lastNeighbourEventNumber) < abs64(distance))
        return getNeighbourEvent(lastNeighbourEvent, neighbourEventNumber - lastNeighbourEventNumber);

    while (event != nullptr && distance != 0) {
        if (distance > 0) {
            distance--;
            event = event->getNextEvent();
        }
        else {
            distance++;
            event = event->getPreviousEvent();
        }
    }

    lastNeighbourEventNumber = neighbourEventNumber;
    lastNeighbourEvent = (IEvent *)event;

    return lastNeighbourEvent;
}

double IEventLog::getApproximatePercentageForEventNumber(eventnumber_t eventNumber)
{
    IEvent *firstEvent = getFirstEvent();
    IEvent *lastEvent = getLastEvent();
    IEvent *event = getEventForEventNumber(eventNumber);

    if (firstEvent == nullptr || firstEvent == lastEvent)
        return 0.0;
    else if (event == nullptr)
        return 0.5;
    else {
        file_offset_t beginOffset = firstEvent->getBeginOffset();
        file_offset_t endOffset = lastEvent->getBeginOffset();

        double percentage = (double)(event->getBeginOffset() - beginOffset) / (endOffset - beginOffset);
        Assert(0.0 <= percentage && percentage <= 1.0);
        return percentage;
    }
}

} // namespace eventlog
}  // namespace omnetpp

