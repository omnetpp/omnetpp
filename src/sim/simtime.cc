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
double SimTime::fscale;
double SimTime::invfscale;

//FIXME overflow should be detected somehow...

void SimTime::setScaleExp(int e)
{
    if (e < -18 || e > 0)
        throw cRuntimeError("simtime_t scale exponent %d is out of accepted range -18..0; "
                            "recommended value is -12 (picosecond resolution with range +-106 days)", e);

    // calculate 10^-e
    scaleexp = e;
    int64 scale = 1;
    while(e++ < 0)
        scale *= 10;

    // store it in double too
    fscale = (double) scale;
    invfscale = 1.0 / fscale;
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

