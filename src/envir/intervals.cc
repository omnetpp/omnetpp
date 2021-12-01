//==========================================================================
//  INTERVALS.CC - part of
//                     OMNeT++/OMNEST
//            Discrete System Simulation in C++
//
//==========================================================================

/*--------------------------------------------------------------*
  Copyright (C) 1992-2017 Andras Varga
  Copyright (C) 2006-2017 OpenSim Ltd.

  This file is distributed WITHOUT ANY WARRANTY. See the file
  `license' for details on this and other legal matters.
*--------------------------------------------------------------*/

#include "common/opp_ctype.h"
#include "common/stringutil.h"
#include "envirbase.h"
#include "intervals.h"

using namespace omnetpp::common;

namespace omnetpp {
namespace envir {

Intervals::Intervals()
{
    intervals = nullptr;
}

Intervals::~Intervals()
{
    delete[] intervals;
}

void Intervals::parse(const char *text)
{
    std::vector<Interval> parsedIntervals;
    for (std::string item : opp_splitandtrim(opp_nulltoempty(text), ",")) {
        // parse interval string
        auto parts = opp_splitandtrim(item, "..");
        if (parts.size() != 2)
            throw cRuntimeError("Wrong syntax in interval %s, [start]..[end] expected", item.c_str());

        // add to vector
        Interval interval;
        if (!parts[0].empty())
            interval.startTime = SimTime::parse(parts[0].c_str());
        if (!parts[1].empty())
            interval.stopTime = SimTime::parse(parts[1].c_str());
        parsedIntervals.push_back(interval);
    }

    // return as plain C++ array
    intervals = new Interval[parsedIntervals.size()+1];  // +1: terminating (0,0)
    for (int i = 0; i < (int)parsedIntervals.size(); i++)
        intervals[i] = parsedIntervals[i];
}

bool Intervals::contains(simtime_t t) const
{
    // empty contains all t values
    if (!intervals)
        return true;

    for (Interval *i = intervals; i->startTime != 0 || i->stopTime != 0; i++)
        if (i->startTime <= t && (i->stopTime == 0 || t <= i->stopTime))
            return true;

    return false;
}

bool Intervals::empty() const
{
    return !intervals || (intervals[0].startTime == 0 && intervals[0].stopTime == 0);
}

}  // namespace envir
}  // namespace omnetpp

