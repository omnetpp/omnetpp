//==========================================================================
//  INTERVALS.CC - part of
//                     OMNeT++/OMNEST
//            Discrete System Simulation in C++
//
//  Author: Levente Meszaros
//
//==========================================================================

/*--------------------------------------------------------------*
  Copyright (C) 1992-2017 Andras Varga
  Copyright (C) 2006-2017 OpenSim Ltd.

  This file is distributed WITHOUT ANY WARRANTY. See the file
  `license' for details on this and other legal matters.
*--------------------------------------------------------------*/

#include "common/opp_ctype.h"
#include "common/stringtokenizer.h"
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
    StringTokenizer tokenizer(text, ",");
    while (tokenizer.hasMoreTokens()) {
        // parse interval string
        const char *s = tokenizer.nextToken();
        const char *ellipsis = strstr(s, "..");
        if (!ellipsis)
            throw cRuntimeError("Wrong syntax in interval %s=%s", text, s);

        const char *startstr = s;
        const char *stopstr = ellipsis+2;
        while (opp_isspace(*startstr))
            startstr++;
        while (opp_isspace(*stopstr))
            stopstr++;

        // add to vector
        Interval interval;
        if (startstr != ellipsis)
            interval.startTime = SimTime::parse(std::string(startstr, ellipsis-startstr).c_str());
        if (*stopstr)
            interval.stopTime = SimTime::parse(stopstr);
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

