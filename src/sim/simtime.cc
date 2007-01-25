//==========================================================================
//   SIMTIME.CC  - part of
//                     OMNeT++/OMNEST
//            Discrete System Simulation in C++
//
//==========================================================================

/*--------------------------------------------------------------*
  Copyright (C) 1992-2006 Andras Varga

  This file is distributed WITHOUT ANY WARRANTY. See the file
  `license' for details on this and other legal matters.
*--------------------------------------------------------------*/

#include <sstream>
#include "simtime.h"
#include "cexception.h"


int SimTime::scaleexp;
int64 SimTime::iscale;
double SimTime::scale_;
double SimTime::invscale_;
double SimTime::maxtime;

//FIXME overflow should be detected somehow...

void SimTime::setScaleExp(int e)
{
    if (e<0 || e>18)
        throw cRuntimeError("simtime_t scale %d is out of accepted range 0..18; "
                            "recommended value is 12 (picosecond resolution, "
                            "range +-106 days)", e);

    // calculate 10^e
    scaleexp = e;
    iscale = 1;
    while(e-- >0)
        iscale *= 10;

    // store it in double too
    scale_ = (double) iscale;
    invscale_ = 1.0 / scale_;

    // and precalculate maxtime as well
    maxtime = ((((int64)1) << 63) - 1) * invscale_;
}

std::string SimTime::str() const
{
    // delegate to operator<<
    std::stringstream out;
    out << *this;
    return out.str();
}

/*XXX
std::string pr(const SimTime& x)
{
   char buf[40];
   char *s;
   sprintf((s=buf+10),"%I64d", x.raw());  // linux: use s=lltostr(x.raw(),buf+sizeof(buf)-1);
   return buf;
}
*/

