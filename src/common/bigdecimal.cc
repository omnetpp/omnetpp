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
#include <assert.h>
#include "bigdecimal.h"

static double zero = 0.0;
static inline int64 max(int64 x, int64 y) { return x > y ? x : y; }
static inline int64 min(int64 x, int64 y) { return x < y ? x : y; }
static inline int64 abs(int64 x) { return x >= 0 ? x : -x; }
static inline int sgn(int64 x) { return (x > 0 ? 1 : (x < 0 ? -1 : 0)); }

void BigDecimal::normalize()
{
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
            if (intVal > _I64_MAX/10)
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

int64 BigDecimal::getDigits(int scale) const
{
    int start = max(scale, this->scale); // inclusive
    int end = min(scale+18, this->scale+18); // exclusive

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
    if (d != d /*NaN*/ || d == 1/zero || d == -1/zero)
        throw opp_runtime_error("BigDecimal::operator=(%d): unexpected value (NaN or infinite)", d);

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
    while (intVal & 1 == 0)
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
            if (intVal <= _I64_MAX/5)
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
            if (intVal <= _I64_MAX/2)
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

    *this = BigDecimal(sign * intVal, scale);
    return *this;
}

bool BigDecimal::operator<(const BigDecimal &x) const
{
    if (scale == x.scale)
        return intVal < x.intVal;
    if (sgn(intVal) < sgn(x.intVal))
        return true;
    if (sgn(intVal) > sgn(x.intVal))
        return false;

    assert(intVal < 0 && x.intVal < 0 || intVal > 0 && x.intVal > 0);
    bool negatives = intVal < 0;

    // compare absolute values by comparing most significant digits first
    bool result = false;
    for (int s = max(scale,x.scale); s > min(scale,x.scale); s-=18)
    {
        int64 digits = this->getDigits(s);
        int64 digitsX = x.getDigits(s);
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
    while (isspace(*p))
        ++p;

    // optional signs
    if (*p == '-')
    {
        sign = -1;
        ++p;
    }
    else if (*p == '+')
        ++p;

    // digits before decimal
    while (isdigit(*p))
    {
        OVERFLOW_CHECK(intVal <= _I64_MAX / 10, s);
        intVal *= 10;
        digit = ((*p++)-'0');
        OVERFLOW_CHECK(intVal <= _I64_MAX - digit, s);
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
        while (isdigit(*p))
        {
            OVERFLOW_CHECK(intVal <= _I64_MAX / 10, s);
            intVal *= 10;
            digit = ((*p++)-'0');
            OVERFLOW_CHECK(intVal <= _I64_MAX - digit, s);
            intVal += digit;
            digits++;
            scale--;
        }
    }

    endp = p;
    return BigDecimal(sign*intVal, scale);
}

