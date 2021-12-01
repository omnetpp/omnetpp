//=========================================================================
//  ENUMS.H - part of
//                  OMNeT++/OMNEST
//           Discrete System Simulation in C++
//
//=========================================================================

/*--------------------------------------------------------------*
  Copyright (C) 2006-2017 OpenSim Ltd.

  This file is distributed WITHOUT ANY WARRANTY. See the file
  `license' for details on this and other legal matters.
*--------------------------------------------------------------*/

#ifndef __OMNETPP_EVENTLOG_ENUMS_H
#define __OMNETPP_EVENTLOG_ENUMS_H

#include "eventlogdefs.h"

namespace omnetpp {
namespace eventlog {

/**
 * Variables which store event numbers throughout the API may have these
 * additional values (positive values are real event numbers).
 */
enum EventNumberKind
{
    /**
     * The event number is not yet calculated before.
     */
    EVENT_NOT_YET_CALCULATED = -1,
    /**
     * There is no such event and will never be because it is impossible. (e.g. the previous event of the first event)
     */
    NO_SUCH_EVENT = -2,
    /**
     * The event is not and will never be in the log, although it could be. (e.g. filtered by the user from the file)
     */
    FILTERED_EVENT = -3,
    /**
     * The event is not yet in the log, but it might appear in the future (e.g. simulation not there yet).
     * The last event number is added to the constant and returned.
     * (e.g. reading the event 1000 when the file ends at 100 will give -104)
     */
    EVENT_NOT_YET_REACHED = -4
};

enum MatchKind
{
    EXACT, // returns not nullptr if exactly one event was found
    FIRST_OR_PREVIOUS, // if multiple matches were found it returns the first one or the previous if there are no matches at all
    FIRST_OR_NEXT, // if multiple matches were found it returns the first one or the next if there are no matches at all
    LAST_OR_NEXT, // as per FIRST_OR_PREVIOUS but the opposite
    LAST_OR_PREVIOUS // as per FIRST_OR_NEXT but the opposite
};

enum TimelineMode {
    SIMULATION_TIME,
    EVENT_NUMBER,
    STEP,
    NONLINEAR
};

}  // namespace eventlog
}  // namespace omnetpp

#endif
