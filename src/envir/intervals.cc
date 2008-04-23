//==========================================================================
//  INTERVALS.CC - part of
//                     OMNeT++/OMNEST
//            Discrete System Simulation in C++
//
//==========================================================================

/*--------------------------------------------------------------*
  Copyright (C) 1992-2005 Andras Varga

  This file is distributed WITHOUT ANY WARRANTY. See the file
  `license' for details on this and other legal matters.
*--------------------------------------------------------------*/

#include "opp_ctype.h"
#include "envirbase.h"
#include "stringtokenizer.h"
#include "intervals.h"

USING_NAMESPACE

Intervals::Intervals()
{
    intervals = NULL;
}

Intervals::~Intervals()
{
    delete [] intervals;
}

void Intervals::parse(const char *text)
{
    std::vector<Interval> parsedIntervals;
    StringTokenizer tokenizer(text, ",");
    while (tokenizer.hasMoreTokens())
    {
        // parse interval string
        const char *s = tokenizer.nextToken();
        const char *ellipsis = strstr(s, "..");
        if (!ellipsis)
            throw cRuntimeError("Wrong syntax in interval %s=%s", text, s);

        const char *startstr = s;
        const char *stopstr = ellipsis+2;
        while (opp_isspace(*startstr)) startstr++;
        while (opp_isspace(*stopstr)) stopstr++;

        // add to vector
        Interval interval;
        if (startstr!=ellipsis)
            interval.startTime = STR_SIMTIME(std::string(startstr, ellipsis-startstr).c_str());
        if (*stopstr)
            interval.stopTime = STR_SIMTIME(stopstr);
        parsedIntervals.push_back(interval);
    }

    // return as plain C++ array
    intervals = new Interval[parsedIntervals.size()+1]; // +1: terminating (0,0)
    for (int i=0; i<(int)parsedIntervals.size(); i++)
        intervals[i] = parsedIntervals[i];
}

bool Intervals::contains(simtime_t t)
{
    // empty contains all t values
    if (!intervals)
        return true;

    for (Interval *i = intervals; i->startTime!=0 || i->stopTime!=0; i++)
        if (i->startTime <= t && (i->stopTime == 0 || t <= i->stopTime))
            return true;

    return false;
}
