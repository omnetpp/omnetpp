//==========================================================================
//   SIMTIME.CC  - part of
//                     OMNeT++/OMNEST
//            Discrete System Simulation in C++
//
//==========================================================================

/*--------------------------------------------------------------*
  Copyright (C) 1992-2015 Andras Varga
  Copyright (C) 2006-2015 OpenSim Ltd.

  This file is distributed WITHOUT ANY WARRANTY. See the file
  `license' for details on this and other legal matters.
*--------------------------------------------------------------*/

#include <sstream>
#include "common/opp_ctype.h"
#include "common/unitconversion.h"
#include "omnetpp/simtime.h"
#include "omnetpp/cexception.h"
#include "omnetpp/cpar.h"
#include "omnetpp/onstartup.h"

using namespace omnetpp::common;

namespace omnetpp {

#define LL    INT64_PRINTF_FORMAT

int SimTime::scaleexp = SimTime::SCALEEXP_UNINITIALIZED;
int64_t SimTime::dscale;
double SimTime::fscale;
double SimTime::invfscale;
int64_t SimTime::maxseconds;
bool SimTime::checkmul = true;

const SimTime SimTime::ZERO;

#define MAX_POWER_OF_TEN  18
static int64_t powersOfTen[MAX_POWER_OF_TEN+1];

static void fillPowersOfTen()
{
    int64_t power = 1;
    for (int i = 0; i <= MAX_POWER_OF_TEN; i++) {
        powersOfTen[i] = power;
        power *= 10;
    }
}

inline int64_t exp10(int exponent)
{
    if (exponent < 0 || exponent > MAX_POWER_OF_TEN)
        return -1;  // error
    return powersOfTen[exponent];
}

void SimTime::setScaleExp(int e)
{
    if (e == scaleexp)
        return;
    if (scaleexp != SCALEEXP_UNINITIALIZED)
        throw cRuntimeError("SimTime::setScaleExp(): Attempt to change the scale exponent after initialization");
    if (e < -18 || e > 0)
        throw cRuntimeError("SimTime scale exponent %d is out of accepted range -18..0", e);

    fillPowersOfTen();

    scaleexp = e;
    dscale = exp10(-scaleexp);
    fscale = (double)dscale;
    invfscale = 1.0 / fscale;
    maxseconds = INT64_MAX / dscale;
}

static std::string range()
{
    return std::string("(-") + SimTime::getMaxTime().str() + "," + SimTime::getMaxTime().str() + ")";
}

void SimTime::initError(double d)
{
    throw cRuntimeError("Global simtime_t variable found, with value %g. Global simtime_t variables are "
                        "forbidden, because scale exponent is not yet known at the time they are initialized. "
                        "Please use double or const_simtime_t instead", d);
}

void SimTime::rangeErrorInt64(double i64)
{
    throw cRuntimeError("Cannot convert %g to simtime_t: out of range %s, allowed by scale exponent %d",
            i64*invfscale, range().c_str(), scaleexp);
}

void SimTime::rangeErrorSeconds(int64_t sec)
{
    throw cRuntimeError("Cannot convert % " LL "d to simtime_t: out of range %s, allowed by scale exponent %d",
            sec, range().c_str(), scaleexp);
}

void SimTime::overflowAdding(const SimTime& x)
{
    t -= x.t;  // restore original value
    throw cRuntimeError("simtime_t overflow adding %s to %s: result is out of range %s, allowed by scale exponent %d",
            x.str().c_str(), str().c_str(), range().c_str(), scaleexp);
}

void SimTime::overflowSubtracting(const SimTime& x)
{
    t += x.t;  // restore original value
    throw cRuntimeError("simtime_t overflow subtracting %s from %s: result is out of range %s, allowed by scale exponent %d",
            x.str().c_str(), str().c_str(), range().c_str(), scaleexp);
}

void SimTime::overflowNegating()
{
    throw cRuntimeError("Error negating simtime_t %s: it is internally represented with INT64_MIN "
            "that has no positive equivalent, try decreasing precision", str().c_str());
}

SimTime::SimTime(int64_t value, SimTimeUnit unit)
{
    if (scaleexp == SCALEEXP_UNINITIALIZED)
        throw cRuntimeError("Global simtime_t variable found, initialized with SimTime(%" LL "d, %d). "
                "Global simtime_t variables are forbidden, because scale exponent is not yet known "
                "at the time they are initialized. Please use double or const_simtime_t instead", value, unit);

    t = value;
    int exponent = unit;
    int expdiff = exponent - scaleexp;
    if (expdiff < 0) {
        int64_t mul = exp10(-expdiff);
        int64_t tmp = t / mul;
        if (mul == -1 || tmp * mul != t)
            throw cRuntimeError("simtime_t error: %" LL "d*10^%d cannot be represented precisely using the current scale exponent %d, "
                    "increase resolution by configuring a smaller scale exponent or use 'double' conversion",
                    value, exponent, scaleexp);
        t = tmp;
    }
    else if (expdiff > 0) {
        int64_t mul = exp10(expdiff);
        t *= mul;
        if (mul == -1 || t / mul != value)
            throw cRuntimeError("simtime_t overflow: cannot represent %" LL "d*10^%d, out of range %s allowed by scale exponent %d",
                    value, exponent, range().c_str(), scaleexp);
    }
}

void SimTime::checkedMul(int64_t x)
{
    int64_t tmp = t * x;
    if (x == 0 || tmp / x == t) {
        t = tmp;
        return;
    }

    throw cRuntimeError("simtime_t overflow multiplying %s by %" LL "d: result is out of range %s, allowed by scale exponent %d",
        str().c_str(), x, range().c_str(), scaleexp);
}

int64_t SimTime::inUnit(SimTimeUnit unit) const
{
    int64_t x = t;
    int exponent = unit;
    int expdiff = exponent - scaleexp;
    if (expdiff > 0) {
        int64_t mul = exp10(expdiff);
        x = (mul == -1) ? 0 : x / mul;
    }
    else if (expdiff < 0) {
        int64_t mul = exp10(-expdiff);
        if (mul == -1 || (x < 0 ? -x : x) >= INT64_MAX / mul)
            throw cRuntimeError("simtime: inUnits(): overflow: cannot represent %s in units of 10^%ds", str().c_str(), exponent);
        x *= mul;
    }
    return x;
}

void SimTime::split(SimTimeUnit unit, int64_t& outValue, SimTime& outRemainder) const
{
    outValue = inUnit(unit);
    outRemainder = *this - SimTime(outValue, unit);
}

const SimTime& SimTime::operator=(const cPar& p)
{
    switch (p.getType()) {
        case cPar::LONG: return operator=(p.longValue());
        case cPar::DOUBLE: return operator=(p.doubleValue());
        default: throw cRuntimeError(&p, "Cannot convert non-numeric parameter to simtime_t");
    }
}

const SimTime& SimTime::operator*=(const cPar& p)
{
    switch (p.getType()) {
        case cPar::LONG: return operator*=(p.longValue());
        case cPar::DOUBLE: return operator*=(p.doubleValue());
        default: throw cRuntimeError(&p, "Cannot convert non-numeric parameter to simtime_t");
    }
}

const SimTime& SimTime::operator/=(const cPar& p)
{
    switch (p.getType()) {
        case cPar::LONG: return operator/=(p.longValue());
        case cPar::DOUBLE: return operator/=(p.doubleValue());
        default: throw cRuntimeError(&p, "Cannot convert non-numeric parameter to simtime_t");
    }
}


std::string SimTime::str() const
{
    // delegate to operator<<
    std::stringstream out;
    out << *this;
    return out.str();
}

char *SimTime::ttoa(char *buf, int64_t t, int scaleexp, char *& endp)
{
    ASSERT(scaleexp <= 0 && scaleexp >= -18);

    // prepare for conversion
    endp = buf + 63;  //19+19+5 should be enough, but play it safe
    *endp = '\0';
    char *s = endp;
    if (t == 0) {
        *--s = '0';
        return s;
    }

    // convert digits
    bool negative = t < 0;
    if (negative)
        t = -t;

    bool skipzeros = true;
    int decimalplace = scaleexp;
    do {
        int64_t res = t / 10;
        int digit = t - (10 * res);

        if (skipzeros && (digit != 0 || decimalplace >= 0))
            skipzeros = false;
        if (decimalplace++ == 0 && s != endp)
            *--s = '.';
        if (!skipzeros)
            *--s = '0' + digit;
        t = res;
    } while (t);

    // add leading zeros, decimal point, etc if needed
    if (decimalplace <= 0) {
        while (decimalplace++ < 0)
            *--s = '0';
        *--s = '.';
        *--s = '0';
    }

    if (negative)
        *--s = '-';
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

const SimTime SimTime::parse(const char *s, const char *& endp)
{
    // find end of the simtime literal in the string
    endp = s;
    while (opp_isspace(*endp))
        endp++;
    if (!*endp)
        {endp = s; return 0;} // it was just space

    while (opp_isalnum(*endp) || opp_isspace(*endp) || *endp == '+' || *endp == '-' || *endp == '.')
        endp++;

    // delegate to the other parse() method
    return parse(std::string(s, endp - s).c_str());
}

}  // namespace omnetpp

