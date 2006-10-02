//=========================================================================
//  IEVENTLOG.H - part of
//                  OMNeT++/OMNEST
//           Discrete System Simulation in C++
//
//=========================================================================

/*--------------------------------------------------------------*
  Copyright (C) 1992-2006 Andras Varga

  This file is distributed WITHOUT ANY WARRANTY. See the file
  `license' for details on this and other legal matters.
*--------------------------------------------------------------*/

#ifndef __IEVENTLOG_H_
#define __IEVENTLOG_H_

#include <sstream>
#include "eventlogdefs.h"
#include "ievent.h"

class IEventLog
{
    public:
        virtual ModuleCreatedEntry *getInitializationModule(int index) = 0;
        virtual int getNumInitializationModules() = 0;

        /**
         * Returns the first event or NULL if the log is empty.
         */
        virtual IEvent *getFirstEvent() = 0;
        /**
         * Returns the last event or NULL if the log is empty.
         */
        virtual IEvent *getLastEvent() = 0;
        /**
         * Returns the requested event or NULL if there's no such event included in the log.
         * The given event number may not be included in the log.
         */
        virtual IEvent *getEventForEventNumber(long eventNumber, MatchKind matchKind = EXACT) = 0;
        /**
         * Returns the requested event or NULL if there's no such event included in the log.
         * The given simulation time may not be included in the log.
         */
        virtual IEvent *getEventForSimulationTime(simtime_t simulationTime, MatchKind matchKind = EXACT) = 0;

        /**
         * Prints initialization entries present before the first event.
         */
        virtual void printInitializationLogEntries(FILE *file = stdout) = 0;

    public:

        /**
         * Returns true if the event with the given event number is included in the log.
         */
        virtual bool isIncludedInLog(long eventNumber) { return getEventForEventNumber(eventNumber) != NULL; }
        virtual IEvent *getFirstEventNotBefore(long eventNumber) { return getEventForEventNumber(eventNumber, LAST); }
        virtual IEvent *getLastEventNotAfter(long eventNumber) { return getEventForEventNumber(eventNumber, FIRST); }

        /**
         * Prints all or only the events in the requested range from the log.
         * The given event numbers may not be included in the log.
         */
        virtual void printEvents(FILE *file = stdout, long fromEventNumber = -1, long toEventNumber = -1);
        /**
         * Prints initialization entries and calls printEvents.
         * The given event numbers may not be included in the log.
         */
        virtual void print(FILE *file = stdout, long fromEventNumber = -1, long toEventNumber = -1);
};

#endif
