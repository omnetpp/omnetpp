//==========================================================================
//   SIMTIME.CC  - part of
//                     OMNeT++/OMNEST
//            Discrete System Simulation in C++
//
//==========================================================================

/*--------------------------------------------------------------*
  Copyright (C) 1992-2008 Andras Varga
  Copyright (C) 2006-2008 OpenSim Ltd.

  This file is distributed WITHOUT ANY WARRANTY. See the file
  `license' for details on this and other legal matters.
*--------------------------------------------------------------*/

#include <sstream>
#include "opp_ctype.h"
#include "simtime.h"
#include "cexception.h"
#include "unitconversion.h"
#include "cpar.h"
#include "onstartup.h"

NAMESPACE_BEGIN

#define LL  INT64_PRINTF_FORMAT

int SimTime::scaleexp = SimTime::SCALEEXP_UNINITIALIZED;
int64 SimTime::dscale;
double SimTime::fscale;
double SimTime::invfscale;

const int SimTime::SCALEEXP_S  = SIMTIME_S;
const int SimTime::SCALEEXP_MS = SIMTIME_MS;
const int SimTime::SCALEEXP_US = SIMTIME_US;
const int SimTime::SCALEEXP_NS = SIMTIME_NS;
const int SimTime::SCALEEXP_PS = SIMTIME_PS;
const int SimTime::SCALEEXP_FS = SIMTIME_FS;

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
    return std::string("(-") + SimTime::getMaxTime().str() + "," + SimTime::getMaxTime().str() + ")";
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

void SimTime::overflowSubtracting(const SimTime& x)
{
    t += x.t; // restore original value
    throw cRuntimeError("simtime_t overflow subtracting %s from %s: result is out of range %s, allowed by scale exponent %d",
                        x.str().c_str(), str().c_str(), range().c_str(), scaleexp);
}

#define MAX_POWER_OF_TEN  18
static int64 powersOfTen[MAX_POWER_OF_TEN+1];

EXECUTE_ON_STARTUP(
    int64 power = 1;
    for (int i = 0; i <= MAX_POWER_OF_TEN; i++) {
        powersOfTen[i] = power;
        power *= 10;
    }
);

inline int64 exp10(int exponent)
{
    if (exponent < 0 || exponent > MAX_POWER_OF_TEN)
        return -1; // error
    return powersOfTen[exponent];
}

SimTime::SimTime(int64 significand, int exponent)
{
    t = significand;
    int expdiff = exponent - scaleexp;
    if (expdiff < 0)
    {
        int64 mul = exp10(-expdiff);
        int64 tmp = t / mul;
        if (mul == -1 || tmp*mul != t)
            throw cRuntimeError("simtime_t error: %" LL "d*10^%d cannot be represented precisely using the current scale exponent %d, "
                    "increase resolution by configuring a smaller scale exponent or use 'double' conversion",
                    significand, exponent, scaleexp);
        t = tmp;
    }
    else if (expdiff > 0)
    {
        int64 mul = exp10(expdiff);
        if (mul == -1 || (t<0?-t:t) >= INT64_MAX / mul)
            throw cRuntimeError("simtime_t overflow: cannot represent %" LL "d*10^%d, out of range %s allowed by scale exponent %d",
                    significand, exponent, range().c_str(), scaleexp);
        t *= mul;
    }
}

int64 SimTime::inUnit(int exponent) const
{
    int64 x = t;
    int expdiff = exponent - scaleexp;
    if (expdiff > 0)
    {
        int64 mul = exp10(expdiff);
        x = (mul == -1) ? 0 : x / mul;
    }
    else if (expdiff < 0)
    {
        int64 mul = exp10(-expdiff);
        if (mul == -1 || (x<0?-x:x) >= INT64_MAX / mul)
            throw cRuntimeError("simtime: inUnits(): overflow: cannot represent %s in units of 10^%ds", str().c_str(), exponent);
        x *= mul;
    }
    return x;
}

void SimTime::split(int exponent, int64& outValue, SimTime& outRemainder) const
{
    outValue = inUnit(exponent);
    outRemainder = *this - SimTime(outValue, exponent);
}

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

NAMESPACE_END

