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

#include "ieventlog.h"

IEventLog::IEventLog()
{
    lastNeighbourEventNumber = -1;
    lastNeighbourEvent = NULL;
    numParsedEvents = 0;
}

void IEventLog::printEvents(FILE *file, long fromEventNumber, long toEventNumber)
{
    IEvent *event = fromEventNumber == -1 ? getFirstEvent() : getFirstEventNotBeforeEventNumber(fromEventNumber);

    while (event != NULL && (toEventNumber == -1 || event->getEventNumber() <= toEventNumber))
    {
        event->print(file);
        event = event->getNextEvent();
    }
}

void IEventLog::print(FILE *file, long fromEventNumber, long toEventNumber)
{
    printInitializationLogEntries(file);
    printEvents(file, fromEventNumber, toEventNumber);
}

IEvent *IEventLog::getNeighbourEvent(IEvent *event, long distance)
{
    long eventNumber = event->getEventNumber() + distance;

    if (lastNeighbourEventNumber != -1 && abs(eventNumber - lastNeighbourEventNumber) < abs(distance))
        return getNeighbourEvent(lastNeighbourEvent, eventNumber - lastNeighbourEventNumber);

    while (event != NULL && distance != 0)
    {
        if (distance > 0) {
            distance--;
            event = event->getNextEvent();
        }
        else {
            distance++;
            event = event->getPreviousEvent();
        }
    }

    lastNeighbourEventNumber = eventNumber;
    lastNeighbourEvent = (IEvent *)event;

    return lastNeighbourEvent;
}

double IEventLog::getApproximatePercentageForEventNumber(long eventNumber)
{
    IEvent *firstEvent = getFirstEvent();
    IEvent *lastEvent = getLastEvent();
    IEvent *event = getEventForEventNumber(eventNumber);

    if (firstEvent == NULL)
        return 0;
    else if (event == NULL)
        return 0.5;
    else {
        long beginOffset = firstEvent->getBeginOffset();
        long endOffset = lastEvent->getEndOffset();

        double percentage = (double)event->getBeginOffset() / (endOffset - beginOffset);

        return std::min(std::max(percentage, 0.0), 1.0);
    }
}
