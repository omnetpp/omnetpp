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
#include "opp_ctype.h"
#include "simtime.h"
#include "cexception.h"
#include "unitconversion.h"
#include "cpar.h"

USING_NAMESPACE


int SimTime::scaleexp = SimTime::SCALEEXP_UNINITIALIZED;
int64 SimTime::dscale;
double SimTime::fscale;
double SimTime::invfscale;

void SimTime::setScaleExp(int e)
{
    if (e == scaleexp)
        return;
    if (scaleexp != SCALEEXP_UNINITIALIZED)
        throw cRuntimeError("SimTime::setScaleExp(): Attempt to change the scale exponent after initialization");

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

static std::string range()
{
    return std::string("(-") + SimTime::maxTime().str() + "," + SimTime::maxTime().str() + ")";
}

void SimTime::initError(double d)
{
    throw cRuntimeError("Global simtime_t variable found, with value %g. Global simtime_t variables are "
                        "forbidden, because scale exponent is not yet known at the time they get initialized. "
                        "Please use double or const_simtime_t instead", d);
}

void SimTime::rangeError(double i64)
{
    throw cRuntimeError("Cannot convert %g to simtime_t: out of range %s, allowed by scale exponent %d",
                        i64*invfscale, range().c_str(), scaleexp);
}

void SimTime::overflowAdding(const SimTime& x)
{
    t -= x.t; // restore original value
    throw cRuntimeError("simtime_t overflow adding %s to %s: result is out of range %s, allowed by scale exponent %d",
                        x.str().c_str(), str().c_str(), range().c_str(), scaleexp);
}

void SimTime::overflowSubstracting(const SimTime& x)
{
    t += x.t; // restore original value
    throw cRuntimeError("simtime_t overflow substracting %s from %s: result is out of range %s, allowed by scale exponent %d",
                        x.str().c_str(), str().c_str(), range().c_str(), scaleexp);
}

//XXX inline, at the end of simtime.h, just after #include "cpar.h" ?
const SimTime& SimTime::operator=(const cPar& p)
{
    return operator=((double)p);
}

const SimTime& SimTime::operator*=(const cPar& p)
{
    return operator*=((double)p);
}

const SimTime& SimTime::operator/=(const cPar& p)
{
    return operator/=((double)p);
}

const SimTime operator*(const SimTime& x, const cPar& p)
{
    return operator*(x, (double)p);
}

const SimTime operator*(const cPar& p, const SimTime& x)
{
    return operator*((double)p, x);
}

const SimTime operator/(const SimTime& x, const cPar& p)
{
    return operator/(x, (double)p);
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
    try {
        // Note: UnitConversion calculates in double, so we may lose precision during conversion
        std::string unit;
        double d = UnitConversion::parseQuantity(s, unit);  // "unit" is OUT parameter
        return unit.empty() ? d : UnitConversion::convertUnit(d, unit.c_str(), "s");
    }
    catch (std::exception& e) {
        throw cRuntimeError("Error converting string \"%s\" to SimTime: %s", s, e.what());
    }
}

const SimTime SimTime::parse(const char *s, const char *&endp)
{
    // find end of the simtime literal in the string
    endp = s;
    while (opp_isspace(*endp))
        endp++;
    if (!*endp)
        {endp = s; return 0;} // it was just space

    while (opp_isalnum(*endp) || opp_isspace(*endp) || *endp=='+' || *endp=='-' || *endp=='.')
        endp++;

    // delegate to the other parse() method
    return parse(std::string(s, endp-s).c_str());
}

