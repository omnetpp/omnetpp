//=========================================================================
//  TESTUTIL.H - part of
//                  OMNeT++/OMNEST
//           Discrete System Simulation in C++
//
//=========================================================================

/*--------------------------------------------------------------*
  Copyright (C) 1992-2015 Andras Varga

  This file is distributed WITHOUT ANY WARRANTY. See the file
  `license' for details on this and other legal matters.
*--------------------------------------------------------------*/

#ifndef _TESTUTIL_H_
#define _TESTUTIL_H_

#include <iostream>
#include <omnetpp/platdep/timeutil.h>

class MeasureTime
{
    timeval start, end;

  public:
    MeasureTime() { gettimeofday(&start, nullptr); }
    ~MeasureTime()
    {
        gettimeofday(&end, nullptr);
        long elapsedTime = timeval_diff_usec(end, start);
        std::cout << "Time: " << elapsedTime/1000 << "ms" << std::endl;
    }
};

#endif

