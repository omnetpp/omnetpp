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
#include "unitconversion.h"
#include "cpar.h"


int SimTime::scaleexp;
int64 SimTime::dscale;
double SimTime::fscale;
double SimTime::invfscale;


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
    dscale = scale;
    fscale = (double) scale;
    invfscale = 1.0 / fscale;
}

//XXX inline, at the end of simtime.h, just after #include "cpar.h" ?
const SimTime& SimTime::operator=(const cPar& d)
{
    return operator=((double)d);
}

std::string SimTime::str() const
{
    // delegate to operator<<
    std::stringstream out;
    out << *this;
    return out.str();
}

char *SimTime::ttoa(char *buf, int64 t, int scaleexp, char *&endp)
{
    ASSERT(scaleexp<=0 && scaleexp>=-18);

    // prepare for conversion
    endp = buf+63;  //19+19+5 should be enough, but play it safe
    *endp = '\0';
    char *s = endp;
    if (t==0)
        {*--s = '0'; return s;}

    // convert digits
    bool negative = t<0;
    if (negative) t = -t;

    bool skipzeros = true;
    int decimalplace = scaleexp;
    do {
        int64 res = t / 10;
        int digit = t - (10*res);

        if (skipzeros && (digit!=0 || decimalplace>=0))
            skipzeros = false;
        if (decimalplace++==0 && s!=endp)
            *--s = '.';
        if (!skipzeros)
            *--s = '0'+digit;
        t = res;
    } while (t);

    // add leading zeros, decimal point, etc if needed
    if (decimalplace<=0)
    {
        while (decimalplace++ < 0)
            *--s = '0';
        *--s = '.';
        *--s = '0';
    }

    if (negative) *--s = '-';
    return s;
}

const SimTime SimTime::parse(const char *s)
{
    // Note: UnitConversion calculates in double, so we may lose some precision during conversion
    return UnitConversion::parseQuantity(s, "s");
}

const SimTime SimTime::parse(const char *s, const char *&endp)
{
    endp = s;
    while (isspace(*endp))
        endp++;
    if (!*endp)
        {endp = s; return 0;} // it was just space

    while (isalnum(*endp) || isspace(*endp) || *endp=='+' || *endp=='-' || *endp=='.')
        endp++;

    // Note: UnitConversion calculates in double, so we may lose some precision during conversion
    return UnitConversion::parseQuantity(std::string(s, endp-s).c_str(), "s");
}

