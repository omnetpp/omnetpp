//==========================================================================
//   BIGDECIMAL.CC  - part of
//                     OMNeT++/OMNEST
//            Discrete System Simulation in C++
//
//  Author: Tamas Borbely
//
//==========================================================================

/*--------------------------------------------------------------*
  Copyright (C) 2006-2008 OpenSim Ltd.

  This file is distributed WITHOUT ANY WARRANTY. See the file
  `license' for details on this and other legal matters.
*--------------------------------------------------------------*/

#include <sstream>
#include <assert.h>
#include <string.h>
#include "opp_ctype.h"
#include "platmisc.h"
#include "bigdecimal.h"

USING_NAMESPACE

// helpers
static double zero = 0.0;
static double dblNaN = zero / zero;
static double dblPositiveInfinity = 1 / zero;
static double dblNegativeInfinity = -1 / zero;
static inline int64 max(int64 x, int64 y) { return x > y ? x : y; }
static inline int64 min(int64 x, int64 y) { return x < y ? x : y; }
static inline int64 abs(int64 x) { return x >= 0 ? x : -x; }
static inline int sgn(int64 x) { return (x > 0 ? 1 : (x < 0 ? -1 : 0)); }
static inline bool isNaN(double d) { return d!=d; }
static inline bool isPositiveInfinity(double d) { return d==dblPositiveInfinity; }
static inline bool isNegativeInfinity(double d) { return d==dblNegativeInfinity; }

BigDecimal BigDecimal::Zero(0, 0);
BigDecimal BigDecimal::One(1, 0);
BigDecimal BigDecimal::MinusOne(-1, 0);
BigDecimal BigDecimal::NaN(0, INT_MAX);
BigDecimal BigDecimal::PositiveInfinity(1, INT_MAX);
BigDecimal BigDecimal::NegativeInfinity(-1, INT_MAX);
BigDecimal BigDecimal::Nil;

void BigDecimal::normalize()
{
    // special values
    if (scale == INT_MAX && (intVal == -1 || intVal == 0 || intVal == 1))
        return;

    // zero
    if (intVal == 0)
    {
        intVal = 0;
        scale = 0;
        return;
    }

    // underflow, XXX should throw an exception?
    if (scale < minScale - INT64_MAX_DIGITS)
    {
        intVal = 0;
        scale = 0;
        return;
    }

    // overflow
    if (scale > maxScale + INT64_MAX_DIGITS)
        throw opp_runtime_error("BigDecimal::normalize(): scale too big: %d.", scale); // XXX should be +-Infinity?

    // transform scale between minScale and maxScale
    if (scale < minScale)
    {
        while (scale < minScale)
        {
            intVal /= 10;
            scale++;

            if (intVal == 0)
            {
                scale = 0;
                return;
            }
        }
    }
    else if (scale > maxScale)
    {
        while (scale > maxScale)
        {
            if (intVal > INT64_MAX/10)
                throw opp_runtime_error("BigDecimal::normalize(): arithmetic overflow");

            intVal *= 10;
            scale--;
        }
    }

    // strip trailing zeros
    while ((intVal % 10 == 0) && scale < maxScale)
    {
        intVal /= 10;
        scale++;
    }
}

int64 BigDecimal::getDigits(int scale, int numDigits) const
{
    assert(!this->isSpecial());

    int start = max(scale, this->scale); // inclusive
    int end = scale+numDigits; // exclusive

    if (start >= end)
        return 0;

    int64 val = abs(this->intVal);
    for (int i = this->scale; i < start; ++i)
        val /= 10;

    if (val == 0)
        return 0;

    int64 result = 0;
    int digit;
    int64 multiplier = 1;
    for (int i = start; i < end; ++i)
    {
        digit = val % 10;
        val /= 10;
        result += multiplier*digit;
        multiplier *= 10;
    }

    for (int i = 0; i < (start-scale); ++i)
        result *= 10;

    return result;
}

const BigDecimal& BigDecimal::operator=(double d)
{
    // check NaN and infinity
    if (::isNaN(d))
        return *this = NaN;
    else if (::isPositiveInfinity(d))
        return *this = PositiveInfinity;
    else if (::isNegativeInfinity(d))
        return *this = NegativeInfinity;

    int sign = 1;
    if (d < 0.0)
    {
        sign = -1;
        d = -d;
    }

    int exponent;
    double mantissa = frexp(d, &exponent); // d = mantissa * 2 ^ exponent

    for (int i=0; i < 52; ++i)
    {
        mantissa *= 2.0;
        exponent--;
    }

    int64 intVal = (int64)mantissa;   // d = intVal * 2 ^ exponent


    // special case for 0.0, next loop would not terminate
    if (intVal == 0)
    {
        this->intVal = 0;
        this->scale = 0;
        return *this;
    }

    // normalize
    while ((intVal & 1) == 0)
    {
        intVal >>= 1;
        exponent++;
    }

    //printf("intVal=%I64d, exponent=%d\n", intVal, exponent);

    int scale;
    if (exponent < 0)
    {
        scale = exponent;
        for (int i = exponent; i < 0; ++i)
        {
            if (intVal <= INT64_MAX/5)
            {
                intVal *= 5;
            }
            else
            {
                intVal /= 2;
                scale++;
            }
        }
    }
    else
    {
        scale = 0;
        for (int i = 0; i < exponent; ++i)
        {
            if (intVal <= INT64_MAX/2)
            {
                intVal *= 2;
            }
            else
            {
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

bool BigDecimal::operator<(const BigDecimal &x) const
{
    if (isSpecial() || x.isSpecial())
    {
        if (isNil() || x.isNil())
            throw opp_runtime_error("BigDecimal::operator<() received Nil.");
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

    // compare absolute values by comparing most significant digits first
    bool result = false;
    for (int s = max(scale,x.scale); s > min(scale,x.scale)-18; s-=18)
    {
        int64 digits = this->getDigits(s, 18);
        int64 digitsX = x.getDigits(s, 18);
        if (digits < digitsX)
        {
            result = true;
            break;
        }
        else if (digits > digitsX)
        {
            result = false;
            break;
        }
    }

    return negatives ? !result : result;
}

double BigDecimal::dbl() const
{
    if (isSpecial())
    {
        if (isNaN())
            return dblNaN;
        else if (*this == PositiveInfinity)
            return dblPositiveInfinity;
        else if (*this == NegativeInfinity)
            return dblNegativeInfinity;
        else // Nil
            throw opp_runtime_error("BigDecimal::dbl(): received Nil."); // XXX should return NaN?
    }

    double d = (double)intVal;
    int s = scale;
    while (s > 0)
    {
        d *= 10.0;
        s--;
    }
    while (s < 0)
    {
        d /= 10.0;
        s++;
    }
    return d;
}

std::string BigDecimal::str() const
{
    // delegate to operator<<
    std::stringstream out;
    out << *this;
    return out.str();
}

char *BigDecimal::ttoa(char *buf, const BigDecimal &x, char *&endp)
{
    // special values
    if (x.isSpecial())
    {
        if (x.isNaN())
        {
            strcpy(buf, "NaN");
            endp = buf+3;
        }
        else if (x == PositiveInfinity)
        {
            strcpy(buf, "+Inf");
            endp = buf+4;
        }
        else if (x == NegativeInfinity)
        {
            strcpy(buf, "-Inf");
            endp = buf+4;
        }
        else // Nil
            throw opp_runtime_error("BigDecimal::ttoa(): received Nil.");
        return buf;
    }

    int64 intVal = x.getIntValue();
    int scale = x.getScale();

    // prepare for conversion
    endp = buf+63;  //19+19+5 should be enough, but play it safe
    *endp = '\0';
    char *s = endp;
    if (intVal==0)
        {*--s = '0'; return s;}

    // convert digits
    bool negative = intVal<0;
    if (negative) intVal = -intVal;

    bool skipzeros = true;
    int decimalplace = scale;
    do {
        int64 res = intVal / 10;
        int digit = intVal - (10*res);

        if (skipzeros && (digit!=0 || decimalplace>=0))
            skipzeros = false;
        if (decimalplace++==0 && s!=endp)
            *--s = '.';
        if (!skipzeros)
            *--s = '0'+digit;
        intVal = res;
    } while (intVal);

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

const BigDecimal BigDecimal::parse(const char *s)
{
    const char *endp;
    return parse(s, endp);
}

#define OVERFLOW_CHECK(c,s) if (!(c)) throw opp_runtime_error("BigDecimal::parse(\"%s\"): arithmetic overflow", (s));


const BigDecimal BigDecimal::parse(const char *s, const char *&endp)
{
    int64 intVal = 0;
    int digit;
    int digits = 0;
    int scale = 0;
    int sign = 1;
    const char *p = s;

    // skip leading spaces
    while (opp_isspace(*p))
        ++p;

    // optional signs
    if (*p == '-')
    {
        sign = -1;
        ++p;
    }
    else if (*p == '+')
        ++p;

    // parse special numbers
    if (opp_isalpha(*p))
    {
        if (strncasecmp(p, "nan", 3) == 0)
        {
            endp = p+3;
            return NaN;
        }
        else if (strncasecmp(p, "inf", 3) == 0) // inf and infinity
        {
            endp = p+3;
            if (strncasecmp(endp, "inity", 5) == 0)
                endp += 5;
            return sign > 0 ? PositiveInfinity : NegativeInfinity;
        }
        else
        {
            endp = p;
            return Zero;  // XXX should return Nil?
        }
    }
    else if (*p=='1' && *(p+1)=='.' && *(p+2)=='#')
    {
        if (strncasecmp(p+3, "ind", 3) == 0)
        {
            endp = p+6;
            return NaN;
        }
        else if (strncasecmp(p+3, "inf", 6) == 0)
        {
            endp = p+6;
            return sign > 0 ? PositiveInfinity : NegativeInfinity;
        }
    }

    // digits before decimal
    while (opp_isdigit(*p))
    {
        OVERFLOW_CHECK(intVal <= INT64_MAX / 10, s);
        intVal *= 10;
        digit = ((*p++)-'0');
        OVERFLOW_CHECK(intVal <= INT64_MAX - digit, s);
        intVal += digit;
        digits++;
    }

    if (digits == 0 && *p != '.')
    {
        throw opp_runtime_error("BigDecimal::parse(\"%s\"): missing digits", s);
    }

    // digits after decimal
    if (*p == '.')
    {
        p++;
        while (opp_isdigit(*p))
        {
            OVERFLOW_CHECK(intVal <= INT64_MAX / 10, s);
            intVal *= 10;
            digit = ((*p++)-'0');
            OVERFLOW_CHECK(intVal <= INT64_MAX - digit, s);
            intVal += digit;
            digits++;
            scale--;
        }
    }

    endp = p;
    return BigDecimal(sign*intVal, scale);
}

