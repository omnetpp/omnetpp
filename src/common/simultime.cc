//==========================================================================
//   SIMULTIME.CC  - part of
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
#include <limits.h>
#include "simultime.h"
#include "unitconversion.h"
#include "exception.h"

char *SimulTime::ttoa(char *buf, simultime_t t, int scaleexp, char *&endp)
{
    assert(scaleexp<=0 && scaleexp>=-18);

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

const simultime_t SimulTime::parse(const char *s, int scaleexp)
{
    char *endp;
    return parse(s, scaleexp, endp);
}

#define OVERFLOW_CHECK(c,s) if (!(c)) throw opp_runtime_error("SimulTime::parse(\"%s\"): arithmetic overflow", (s));

const simultime_t SimulTime::parse(const char *s, int scaleexp, const char *&endp)
{
    int64 result = 0;
    int digit;
    int digits = 0;
    int log10Exp = 0;
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
        OVERFLOW_CHECK(result <= _I64_MAX / 10, s);
        result *= 10;
        digit = ((*p++)-'0');
        OVERFLOW_CHECK(result <= _I64_MAX - digit, s);
        result += digit;
        digits++;
    }

    if (digits == 0 && *p != '.')
    {
        throw opp_runtime_error("SimulTime::parse(\"%s\"): missing digits", s);
    }

    // digits after decimal
    if (*p == '.')
    {
        p++;
        while (isdigit(*p))
        {
            OVERFLOW_CHECK(result <= _I64_MAX / 10, s);
            result *= 10;
            digit = ((*p++)-'0');
            OVERFLOW_CHECK(result <= _I64_MAX - digit, s);
            result += digit;
            digits++;
            log10Exp--;
        }
    }

    // adjust according to the required scale
    if (log10Exp < scaleexp)
    {
        while (log10Exp < scaleexp)
        {
            result /= 10;
            log10Exp++;
        }
    }
    else if (log10Exp > scaleexp)
    {
        while (log10Exp > scaleexp)
        {
            OVERFLOW_CHECK(result <= _I64_MAX / 10, s);
            result *= 10;
            log10Exp--;
        }
    }

    endp = p;
    return sign*result;
}

