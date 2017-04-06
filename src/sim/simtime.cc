//==========================================================================
//   SIMTIME.CC  - part of
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

#include <sstream>
#include "common/opp_ctype.h"
#include "common/unitconversion.h"
#include "common/stringutil.h"
#include "omnetpp/simtime.h"
#include "omnetpp/cexception.h"
#include "omnetpp/cpar.h"
#include "omnetpp/onstartup.h"

using namespace omnetpp::common;

namespace omnetpp {

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
    throw cRuntimeError("Cannot convert %g to simtime_t: Out of range %s, allowed by scale exponent %d",
            i64*invfscale, range().c_str(), scaleexp);
}

void SimTime::rangeErrorSeconds(int64_t sec)
{
    throw cRuntimeError("Cannot convert % " PRId64 "D to simtime_t: Out of range %s, allowed by scale exponent %d",
            sec, range().c_str(), scaleexp);
}

void SimTime::overflowAdding(const SimTime& x)
{
    t -= x.t;  // restore original value
    throw cRuntimeError("simtime_t overflow adding %s to %s: Result is out of range %s, allowed by scale exponent %d",
            x.str().c_str(), str().c_str(), range().c_str(), scaleexp);
}

void SimTime::overflowSubtracting(const SimTime& x)
{
    t += x.t;  // restore original value
    throw cRuntimeError("simtime_t overflow subtracting %s from %s: Result is out of range %s, allowed by scale exponent %d",
            x.str().c_str(), str().c_str(), range().c_str(), scaleexp);
}

void SimTime::overflowNegating()
{
    throw cRuntimeError("Cannot negate simtime_t %s: It is internally represented with INT64_MIN "
            "that has no positive equivalent (try decreasing precision)", str().c_str());
}

SimTime::SimTime(int64_t value, SimTimeUnit unit)
{
    if (scaleexp == SCALEEXP_UNINITIALIZED)
        throw cRuntimeError("Global simtime_t variable found, initialized with SimTime(%" PRId64 ", %d). "
                "Global simtime_t variables are forbidden, because scale exponent is not yet known "
                "at the time they are initialized. Please use double or const_simtime_t instead", value, unit);

    t = value;
    int exponent = unit;
    int expdiff = exponent - scaleexp;
    if (expdiff < 0) {
        int64_t mul = exp10(-expdiff);
        int64_t tmp = t / mul;
        if (mul == -1 || tmp * mul != t)
            throw cRuntimeError("simtime_t: %" PRId64 "*10^%d cannot be represented precisely using the current scale exponent %d, "
                    "increase resolution by configuring a smaller scale exponent or use 'double' conversion",
                    value, exponent, scaleexp);
        t = tmp;
    }
    else if (expdiff > 0) {
        int64_t mul = exp10(expdiff);
        t *= mul;
        if (mul == -1 || t / mul != value)
            throw cRuntimeError("simtime_t overflow: Cannot represent %" PRId64 "*10^%d, out of range %s allowed by scale exponent %d",
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

    throw cRuntimeError("simtime_t overflow multiplying %s by %" PRId64 ": Result is out of range %s, allowed by scale exponent %d",
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
            throw cRuntimeError("SimTime::inUnit(): Overflow: Cannot represent %s in units of 10^%ds", str().c_str(), exponent);
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

double operator/(const cPar& p, const SimTime& x)
{
    return p.doubleValue() / x.dbl();
}

char *SimTime::ttoa(char *buf, int64_t t, int scaleexp, char *& endp)
{
    return opp_ttoa(buf, t, scaleexp, endp);
}

std::string SimTime::format(int prec, const char *decimalSep, const char *digitSep, bool addUnits, const char *beforeUnit, const char *afterUnit) const
{
    ASSERT(scaleexp <= 0 && scaleexp >= -18);

    if (prec > 0 || prec < -18)
        throw cRuntimeError("SimTime::format(): prec=%d out of range, must be in 0..-18", prec);

    if (digitSep && !*digitSep) digitSep = nullptr;
    if (!beforeUnit) beforeUnit = "";
    if (!afterUnit) afterUnit = "";

    std::stringstream out;
    if (t < 0)
        out << "-";

    char digits[32];
    sprintf(digits, "%" PRId64, t<0 ? -t : t);
    int numDigits = strlen(digits);

    int startDecimal = scaleexp + numDigits - 1;
    int endDecimal = prec;

    if (startDecimal < 0)
        startDecimal = 0;  // always print seconds
    if (endDecimal > 0)
        endDecimal = 0;  // always print seconds
    if ((endDecimal % 3) != 0 && (addUnits || digitSep))
        endDecimal = 3*((endDecimal-2)/3); // make it multiple of 3

    const char *units[] = { "s", "ms", "us", "ns", "ps", "fs", "as" };

    for (int decimalPlace = startDecimal; decimalPlace >= endDecimal; decimalPlace--) {
        int index = (scaleexp + numDigits - 1) - decimalPlace;
        out << ((index < 0 || index >= numDigits) ? '0' : digits[index]);
        if (decimalPlace % 3 == 0) {
            if (addUnits && decimalPlace <= 0 && decimalPlace >= -18) {
                out << beforeUnit << units[-decimalPlace/3] << afterUnit;
            }
            else if (decimalPlace == 0) {
                if (endDecimal < 0)
                    out << decimalSep;
            }
            else if (digitSep && decimalPlace != endDecimal) {
                out << digitSep;
            }
        }
    }

    return out.str();
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
        throw cRuntimeError("Cannot convert string \"%s\" to SimTime: %s", s, e.what());
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

