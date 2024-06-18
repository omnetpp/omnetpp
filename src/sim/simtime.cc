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
#include <limits>
#include <cinttypes>  // PRId64
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

static const char *unitNames[] = { "s", "ms", "us", "ns", "ps", "fs", "as" };

static void fillPowersOfTen()
{
    int64_t power = 1;
    for (int i = 0; i <= MAX_POWER_OF_TEN; i++) {
        if (i != 0)
            power *= 10;
        powersOfTen[i] = power;
    }
}

static struct PowersOfTenInitializer {
    PowersOfTenInitializer() {fillPowersOfTen();}
} dummy;


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

void SimTime::rangeErrorInt64(double d)
{
    throw cRuntimeError("simtime_t: Cannot represent %g, out of the range %s allowed by the current scale exponent %d",
            d*invfscale, range().c_str(), scaleexp);
}

void SimTime::rangeErrorSeconds(int64_t sec)
{
    throw cRuntimeError("simtime_t: Cannot represent % " PRId64 "D, out of the range %s allowed by the current scale exponent %d",
            sec, range().c_str(), scaleexp);
}

void SimTime::overflowAdding(const SimTime& x)
{
    t -= x.t;  // restore original value
    throw cRuntimeError("simtime_t: overflow adding %s to %s: Result is out of the range %s allowed by the current scale exponent %d",
            x.str().c_str(), str().c_str(), range().c_str(), scaleexp);
}

void SimTime::overflowSubtracting(const SimTime& x)
{
    t += x.t;  // restore original value
    throw cRuntimeError("simtime_t: overflow subtracting %s from %s: Result is out of the range %s allowed by the current scale exponent %d",
            x.str().c_str(), str().c_str(), range().c_str(), scaleexp);
}

void SimTime::overflowNegating()
{
    throw cRuntimeError("simtime_t: Cannot negate simtime_t %s: It is internally represented with INT64_MIN "
            "that has no positive equivalent (try decreasing precision)", str().c_str());
}

void SimTime::fromInt64WithUnit(int64_t value, SimTimeUnit unit)
{
    if (scaleexp == SCALEEXP_UNINITIALIZED)
        throw cRuntimeError("Global simtime_t variable found, initialized with SimTime(%" PRId64 ", %d). "
                "Global simtime_t variables are forbidden, because scale exponent is not yet known "
                "at the time they are initialized. Please use double or const_simtime_t instead", value, unit);

    int exponent = unit;
    int expdiff = exponent - scaleexp;
    if (expdiff < 0) {
        int64_t mul = exp10(-expdiff);
        t = value / mul;
        if (mul == -1 || t * mul != value)
            throw cRuntimeError("simtime_t: Cannot represent %" PRId64 "*10^%d using the current scale exponent %d, "
                    "increase resolution by configuring a smaller scale exponent or use 'double' conversion",
                    value, exponent, scaleexp);
    }
    else if (expdiff > 0) {
        int64_t mul = exp10(expdiff);
        t = value * mul;
        if (mul == -1 || t / mul != value)
            throw cRuntimeError("simtime_t overflow: Cannot represent %" PRId64 "*10^%d, out of range %s allowed by scale exponent %d",
                    value, exponent, range().c_str(), scaleexp);
    }
    else {
        t = value;
    }
}

void SimTime::fromUint64WithUnit(uint64_t value, SimTimeUnit unit)
{
    if (scaleexp == SCALEEXP_UNINITIALIZED)
        throw cRuntimeError("Global simtime_t variable found, initialized with SimTime(%" PRIu64 ", %d). "
                "Global simtime_t variables are forbidden, because scale exponent is not yet known "
                "at the time they are initialized. Please use double or const_simtime_t instead", value, unit);

    int exponent = unit;
    int expdiff = exponent - scaleexp;
    uint64_t tmp;
    if (expdiff < 0) {
        uint64_t mul = (uint64_t)exp10(-expdiff);
        tmp = value / mul;
        if (mul == (uint64_t)-1 || tmp * mul != value)
            throw cRuntimeError("simtime_t: Cannot represent %" PRIu64 "*10^%d precisely using the current scale exponent %d, "
                    "increase resolution by configuring a smaller scale exponent or use 'double' conversion",
                    value, exponent, scaleexp);
    }
    else if (expdiff > 0) {
        uint64_t mul = (uint64_t)exp10(expdiff);
        tmp = value * mul;
        if (mul == -1 || tmp / mul != value)
            throw cRuntimeError("simtime_t overflow: Cannot represent %" PRIu64 "*10^%d, out of range %s allowed by scale exponent %d",
                    value, exponent, range().c_str(), scaleexp);
    }
    else {
        tmp = value;
    }

    t = fromUint64(tmp);
}

static bool isAlmostInteger(double value)
{
    // Returns true if value is an integer, or only 1 ULP (the smallest representable unit) away from an integer.
    // This function is needed due to limitations of IEEE floating point; e.g. 8.7*1e12 is NOT an integer
    // (x == floor(x) does not hold for it), but it is an "almost integer", and will yield the correct
    // int64_t when rounded.
    double nearestInt = std::round(value);
    double ulp = std::nextafter(nearestInt, nearestInt + 1.0) - nearestInt;
    return std::fabs(value - nearestInt) <= ulp;
}

SimTime::SimTime(double value, SimTimeUnit unit, bool allowRounding)
{
    assertInited(value);
    int exponent = unit;
    int expdiff = exponent - scaleexp;
    double tmp = expdiff < 0 ? value / pow(10,-expdiff) : value * pow(10,expdiff);
    if (!allowRounding && tmp != floor(tmp) && !isAlmostInteger(tmp))
        throw cRuntimeError("simtime_t: Cannot represent %g*10^%d precisely using the current scale exponent %d",
                value, exponent, scaleexp);
    t = toInt64(tmp);
}

void SimTime::checkedMul(int64_t x)
{
    int64_t tmp = t * x;
    if (x == 0 || tmp / x == t) {
        t = tmp;
        return;
    }

    throw cRuntimeError("simtime_t: overflow multiplying %s by %" PRId64 ": Result is out of the range %s allowed by the current scale exponent %d",
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

SimTime& SimTime::operator=(const cPar& p)
{
    switch (p.getType()) {
        case cPar::INT: return operator=(p.intValue());
        case cPar::DOUBLE: return operator=(p.doubleValue());
        default: throw cRuntimeError(&p, "Cannot use non-numeric parameter in arithmetic operations with simtime_t");
    }
}

const SimTime& SimTime::operator*=(const cPar& p)
{
    switch (p.getType()) {
        case cPar::INT: return operator*=(p.intValue());
        case cPar::DOUBLE: return operator*=(p.doubleValue());
        default: throw cRuntimeError(&p, "Cannot use non-numeric parameter in arithmetic operations with simtime_t");
    }
}

const SimTime& SimTime::operator/=(const cPar& p)
{
    switch (p.getType()) {
        case cPar::INT: return operator/=(p.intValue());
        case cPar::DOUBLE: return operator/=(p.doubleValue());
        default: throw cRuntimeError(&p, "Cannot use non-numeric parameter in arithmetic operations with simtime_t");
    }
}

static int64_t gcd(int64_t a, int64_t b)
{
    while (b != 0) {
        int64_t tmp = a % b;
        a = b;
        b = tmp;
    }
    return a;
}

double operator/(long long x, const SimTime& y)
{
    // Try to compute it precisely, as x * 10^scaleexp / y.raw().
    //
    // Pitfall: (x * 10^scaleexp) might overflow. We can mitigate that trying
    // to simplify the fraction by dividing both numerator and denominator by their GCD.
    // If numerator still overflows, perform computation in double.
    // If denominator becomes 1, return numerator cast to double.
    // Otherwise perform division using double arithmetic, and return the result.

    int64_t num1 = x;
    int64_t num2 = powersOfTen[-SimTime::scaleexp];
    int64_t denom = y.t;

    int64_t num = num1*num2;
    if (num / num2 != num1) { // overflow, try simplification
        if (denom == 0)
            return (double)num1 * (double)num2 / double(denom);
        int64_t gcd1 = gcd(num1, denom);
        num1 /= gcd1; denom /= gcd1;

        int64_t gcd2 = gcd(num2, denom);
        num2 /= gcd2; denom /= gcd2;

        num = num1*num2;
        if (num / num2 != num1) // still overflow, fall back to double
            return (double)num1 * (double)num2 / double(denom);
    }

    return (denom == 1) ? (double)num : (double)num / (double)denom;
}

double operator/(unsigned long long x, const SimTime& y)
{
    if (x <= std::numeric_limits<long long>::max())
        return operator/((long long)x, y);
    else if ((x&1) == 0 || x+1 == 0)
        return 2.0 * operator/((long long)(x/2), y);
    else
        return 2.0 * operator/((long long)(x/2+1), y); // round up x/2 (unless it's maxint, see x+1==0 condition above)
}

double operator/(const cPar& p, const SimTime& x)
{
    switch (p.getType()) {
        case cPar::INT: return p.intValue() / x;
        case cPar::DOUBLE: return p.doubleValue() / x;
        default: throw cRuntimeError(&p, "Cannot use non-numeric parameter in arithmetic operations with simtime_t");
    }
}

char *SimTime::ttoa(char *buf, int64_t t, int scaleexp, char *& endp)
{
    return opp_ttoa(buf, t, scaleexp, endp);
}

std::string SimTime::ustr() const
{
    if (t == 0)
        return "0s";

    // compute ~abs(t)
    int64_t tt = t;
    if (tt < 0) {
        tt = -tt;
        if (tt < 0)
            tt = -(tt+1);
    }

    // determine unit to print in (seconds and smaller units are considered)
    int unitExp = 0;
    while (unitExp > scaleexp && tt < powersOfTen[-scaleexp+unitExp])
        unitExp -= 3;
    return ustr((SimTimeUnit)unitExp);
}

std::string SimTime::ustr(SimTimeUnit unit) const
{
    int unitExp = (int)unit;
    char buf[80];
    char *endp;
    const char *result = opp_ttoa(buf, t, scaleexp-unitExp, endp);
    strcpy(endp, unitNames[-unitExp/3]);
    return result;
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
    snprintf(digits, sizeof(digits), "%" PRId64, t<0 ? -t : t);
    int numDigits = strlen(digits);

    int startDecimal = scaleexp + numDigits - 1;
    int endDecimal = prec;

    if (startDecimal < 0)
        startDecimal = 0;  // always print seconds
    if (endDecimal > 0)
        endDecimal = 0;  // always print seconds
    if ((endDecimal % 3) != 0 && (addUnits || digitSep))
        endDecimal = 3*((endDecimal-2)/3); // make it multiple of 3


    for (int decimalPlace = startDecimal; decimalPlace >= endDecimal; decimalPlace--) {
        int index = (scaleexp + numDigits - 1) - decimalPlace;
        out << ((index < 0 || index >= numDigits) ? '0' : digits[index]);
        if (decimalPlace % 3 == 0) {
            if (addUnits && decimalPlace <= 0 && decimalPlace >= -18) {
                out << beforeUnit << unitNames[-decimalPlace/3] << afterUnit;
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

