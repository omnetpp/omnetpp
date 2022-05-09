//==========================================================================
//   BIGDECIMAL.CC  - part of
//                     OMNeT++/OMNEST
//            Discrete System Simulation in C++
//
//==========================================================================

/*--------------------------------------------------------------*
  Copyright (C) 2006-2017 OpenSim Ltd.

  This file is distributed WITHOUT ANY WARRANTY. See the file
  `license' for details on this and other legal matters.
*--------------------------------------------------------------*/

#include <sstream>
#include <cassert>
#include <cstring>
#include <algorithm>
#include "omnetpp/platdep/platmisc.h"
#include "opp_ctype.h"
#include "bigdecimal.h"
#include "commonutil.h"  // NaN & friends

namespace omnetpp {
namespace common {

// helpers
static inline int sgn(int64_t x) { return x > 0 ? 1 : (x < 0 ? -1 : 0); }

const BigDecimal BigDecimal::Zero(0, 0);
const BigDecimal BigDecimal::One(1, 0);
const BigDecimal BigDecimal::MinusOne(-1, 0);
const BigDecimal BigDecimal::NaN(0, INT_MAX);
const BigDecimal BigDecimal::PositiveInfinity(1, INT_MAX);
const BigDecimal BigDecimal::NegativeInfinity(-1, INT_MAX);
const BigDecimal BigDecimal::Nil;

static int64_t powersOfTen[19];
static double negativePowersOfTen[21];

class PowersOfTenInitializer
{
  public:
    PowersOfTenInitializer();
};

static PowersOfTenInitializer initializer;

PowersOfTenInitializer::PowersOfTenInitializer()
{
    int64_t power = 1;
    for (unsigned int i = 0; i < sizeof(powersOfTen) / sizeof(*powersOfTen); i++) {
        if (i != 0)
            power *= 10;
        powersOfTen[i] = power;
    }

    double negativePower = 1;
    for (unsigned int i = 0; i < sizeof(negativePowersOfTen) / sizeof(*negativePowersOfTen); i++) {
        negativePowersOfTen[i] = negativePower;
        negativePower /= 10.0;
    }
}

void BigDecimal::normalize()
{
    // special values
    if (scale == INT_MAX && (intVal == -1 || intVal == 0 || intVal == 1))
        return;

    // zero
    if (intVal == 0) {
        intVal = 0;
        scale = 0;
        return;
    }

    // underflow, XXX should throw an exception?
    if (scale < minScale - INT64_MAX_DIGITS) {
        intVal = 0;
        scale = 0;
        return;
    }

    // overflow
    if (scale > maxScale + INT64_MAX_DIGITS)
        throw opp_runtime_error("BigDecimal: normalize(): Scale %d is too big", scale);  // XXX should be +-Infinity?

    // transform scale between minScale and maxScale
    if (scale < minScale) {
        while (scale < minScale) {
            intVal /= 10;
            scale++;

            if (intVal == 0) {
                scale = 0;
                return;
            }
        }
    }
    else if (scale > maxScale) {
        while (scale > maxScale) {
            if (intVal > INT64_MAX/10)
                throw opp_runtime_error("BigDecimal: normalize(): Arithmetic overflow");

            intVal *= 10;
            scale--;
        }
    }

    // strip trailing zeros
    while ((intVal % 10 == 0) && scale < maxScale) {
        intVal /= 10;
        scale++;
    }
}

BigDecimal& BigDecimal::operator=(double d)
{
    // check NaN and infinity
    if (omnetpp::common::isNaN(d))
        return *this = NaN;
    else if (omnetpp::common::isPositiveInfinity(d))
        return *this = PositiveInfinity;
    else if (omnetpp::common::isNegativeInfinity(d))
        return *this = NegativeInfinity;

    int sign = 1;
    if (d < 0.0) {
        sign = -1;
        d = -d;
    }

    int exponent;
    double mantissa = frexp(d, &exponent);  // d = mantissa * 2 ^ exponent

    for (int i = 0; i < 52; ++i) {
        mantissa *= 2.0;
        exponent--;
    }

    int64_t intVal = (int64_t)mantissa;  // d = intVal * 2 ^ exponent

    // special case for 0.0, next loop would not terminate
    if (intVal == 0) {
        this->intVal = 0;
        this->scale = 0;
        return *this;
    }

    // normalize
    while ((intVal & 1) == 0) {
        intVal >>= 1;
        exponent++;
    }

    // printf("intVal=%I64d, exponent=%d\n", intVal, exponent);

    int scale;
    if (exponent < 0) {
        scale = exponent;
        for (int i = exponent; i < 0; ++i) {
            if (intVal <= INT64_MAX/5) {
                intVal *= 5;
            }
            else {
                intVal /= 2;
                scale++;
            }
        }
    }
    else {
        scale = 0;
        for (int i = 0; i < exponent; ++i) {
            if (intVal <= INT64_MAX/2) {
                intVal *= 2;
            }
            else {
                intVal /= 5;
                scale++;
            }
        }
    }

    this->intVal = sign * intVal;
    this->scale = scale;
    this->normalize();
    return *this;
}

bool BigDecimal::operator<(const BigDecimal& x) const
{
    if (isSpecial() || x.isSpecial()) {
        if (isNil() || x.isNil())
            throw opp_runtime_error("BigDecimal: operator< received Nil");
        else if (isNaN() || x.isNaN())
            return false;
        else if (x == PositiveInfinity)
            return *this != PositiveInfinity;
        else if (*this == NegativeInfinity)
            return x != NegativeInfinity;
        else
            return false;
    }

    if (scale == x.scale)
        return intVal < x.intVal;
    if (sgn(intVal) < sgn(x.intVal))
        return true;
    if (sgn(intVal) > sgn(x.intVal))
        return false;

    assert((intVal < 0 && x.intVal < 0) || (intVal > 0 && x.intVal > 0));
    bool negatives = intVal < 0;
    int minScale = std::min(scale, x.scale);
    int m = scale - minScale;
    int xm = x.scale - minScale;
    const int NUMPOWERS = sizeof(powersOfTen) / sizeof(*powersOfTen);
    assert(m < NUMPOWERS && xm < NUMPOWERS);
    int64_t v = intVal;
    if (m != 0) {
        int64_t mp = powersOfTen[m];
        v = intVal * mp;
        if (v / mp != intVal)
            // overflow
            return negatives;
    }
    int64_t xv = x.intVal;
    if (xm != 0) {
        int64_t xmp = powersOfTen[xm];
        xv = x.intVal * xmp;
        if (xv / xmp != x.intVal)
            // overflow
            return !negatives;
    }
    return v < xv;
}

int64_t BigDecimal::getMantissaForScale(int reqScale) const
{
    if (isSpecial())
        throw opp_runtime_error("BigDecimal: Cannot return mantissa for Nil, NaN or +/-Inf value");
    checkScale(reqScale);
    int scaleDiff = scale - reqScale;
    if (scaleDiff == 0)
        return intVal;
    else if (scaleDiff < 0)
        return intVal / powersOfTen[-scaleDiff];
    else
        return intVal * powersOfTen[scaleDiff];
}

double BigDecimal::dbl() const
{
    if (isSpecial()) {
        if (*this == PositiveInfinity)
            return POSITIVE_INFINITY;
        else if (*this == NegativeInfinity)
            return NEGATIVE_INFINITY;
        else  // NaN or Nil
            return omnetpp::common::NaN;
    }

    return (double)intVal * negativePowersOfTen[-scale];
}

std::string BigDecimal::str() const
{
    // delegate to operator<<
    std::stringstream out;
    out << *this;
    return out.str();
}

char *BigDecimal::ttoa(char *buf, const BigDecimal& x, char *& endp)
{
    // special values
    if (x.isSpecial()) {
        if (x.isNaN()) {
            strcpy(buf, "NaN");
            endp = buf+3;
        }
        else if (x == PositiveInfinity) {
            strcpy(buf, "+Inf");
            endp = buf+4;
        }
        else if (x == NegativeInfinity) {
            strcpy(buf, "-Inf");
            endp = buf+4;
        }
        else {
            strcpy(buf, "Nil");
            endp = buf+3;
        }
        return buf;
    }

    int64_t intVal = x.getIntValue();
    int scale = x.getScale();

    // prepare for conversion
    endp = buf+63;  // 19+19+5 should be enough, but play it safe
    *endp = '\0';
    char *s = endp;
    if (intVal == 0) {
        *--s = '0';
        return s;
    }

    // convert digits
    bool negative = intVal < 0;
    if (negative)
        intVal = -intVal;

    bool skipzeros = true;
    int decimalplace = scale;
    do {
        int64_t res = intVal / 10;
        int digit = intVal - (10*res);

        if (skipzeros && (digit != 0 || decimalplace >= 0))
            skipzeros = false;
        if (decimalplace++ == 0 && s != endp)
            *--s = '.';
        if (!skipzeros)
            *--s = '0'+digit;
        intVal = res;
    } while (intVal);

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

const BigDecimal BigDecimal::parse(const char *s)
{
    const char *endp;
    BigDecimal value = parse(s, endp);
    if (*endp != '\0')
        throw opp_runtime_error("BigDecimal: Invalid number syntax '%s'", s);
    return value;
}

#define OVERFLOW_CHECK(c, s)    if (!(c)) \
        throw opp_runtime_error("BigDecimal: Arithmetic overflow while parsing '%s'", (s));

const BigDecimal BigDecimal::parse(const char *s, const char *& endp)
{
    int64_t intVal = 0;
    int digits = 0;
    int scale = 0;
    int sign = 1;
    const char *p = s;

    // check for slow path
    if (!opp_isdigit(*p)) {
        // skip leading spaces
        while (opp_isspace(*p))
            ++p;

        // optional signs
        if (*p == '-') {
            sign = -1;
            ++p;
        }
        else if (*p == '+')
            ++p;

        // parse special numbers
        if (opp_isalpha(*p)) {
            if (strncasecmp(p, "nan", 3) == 0) {
                endp = p+3;
                return NaN;
            }
            else if (strncasecmp(p, "inf", 3) == 0) {  // inf and infinity
                endp = p+3;
                if (strncasecmp(endp, "inity", 5) == 0)
                    endp += 5;
                return sign > 0 ? PositiveInfinity : NegativeInfinity;
            }
            else {
                endp = p;
                return Zero;  // XXX should return Nil?
            }
        }
    }
    if (*p == '1' && *(p+1) == '.' && *(p+2) == '#') {
        if (strncasecmp(p+3, "ind", 3) == 0) {
            endp = p+6;
            return NaN;
        }
        else if (strncasecmp(p+3, "inf", 3) == 0) {
            endp = p+6;
            return sign > 0 ? PositiveInfinity : NegativeInfinity;
        }
    }

    // digits before decimal
    while (opp_isdigit(*p)) {
        OVERFLOW_CHECK(intVal <= INT64_MAX / 10, s);
        intVal = intVal * 10 + ((*p++)-'0');
        OVERFLOW_CHECK(intVal >= 0, s);
        digits++;
    }

    if (digits == 0 && *p != '.') {
        throw opp_runtime_error("BigDecimal: Invalid number syntax '%s'", s);
    }

    // digits after decimal
    if (*p == '.') {
        p++;
        while (opp_isdigit(*p)) {
            OVERFLOW_CHECK(intVal <= INT64_MAX / 10, s);
            intVal = intVal * 10 + ((*p++)-'0');
            OVERFLOW_CHECK(intVal >= 0, s);
            scale--;
        }
    }

    endp = p;
    return BigDecimal(sign*intVal, scale);
}

const BigDecimal operator+(const BigDecimal& x, const BigDecimal& y)
{
    // 1. try to add exactly
    int scale = std::min(x.scale, y.scale);
    int xm = x.scale - scale;
    int ym = y.scale - scale;

    const int NUMPOWERS = sizeof(powersOfTen) / sizeof(*powersOfTen);

    if (!x.isSpecial() && !y.isSpecial() && 0 <= xm && xm < NUMPOWERS && 0 <= ym && ym < NUMPOWERS) {
        int64_t xmp = powersOfTen[xm];
        int64_t xv = x.intVal * xmp;

        if (xv / xmp == x.intVal) {
            int64_t ymp = powersOfTen[ym];
            int64_t yv = y.intVal * ymp;

            if (yv / ymp == y.intVal) {
                bool sameSign = haveSameSign(xv, yv);
                int64_t intVal = xv + yv;

                if (!sameSign || haveSameSign(intVal, yv))
                    return BigDecimal(intVal, scale);
            }
        }
    }

    // 2. add with precision loss
    return BigDecimal(x.dbl()+y.dbl());
}

const BigDecimal operator-(const BigDecimal& x, const BigDecimal& y)
{
    // 1. try to subtract exactly
    int scale = std::min(x.scale, y.scale);
    int xm = x.scale - scale;
    int ym = y.scale - scale;

    const int NUMPOWERS = sizeof(powersOfTen) / sizeof(*powersOfTen);

    if (!x.isSpecial() && !y.isSpecial() && 0 <= xm && xm < NUMPOWERS && 0 <= ym && ym < NUMPOWERS) {
        int64_t xmp = powersOfTen[xm];
        int64_t xv = x.intVal * xmp;

        if (xv / xmp == x.intVal) {
            int64_t ymp = powersOfTen[ym];
            int64_t yv = y.intVal * ymp;

            if (yv / ymp == y.intVal) {
                bool differentSign = !haveSameSign(xv, yv);
                int64_t intVal = xv - yv;

                if (!differentSign || !haveSameSign(intVal, yv))
                    return BigDecimal(intVal, scale);
            }
        }
    }

    // 2. subtract with precision loss
    return BigDecimal(x.dbl()-y.dbl());
}

}  // namespace common
}  // namespace omnetpp

