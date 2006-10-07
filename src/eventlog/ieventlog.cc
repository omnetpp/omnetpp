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
