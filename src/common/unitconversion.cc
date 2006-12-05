//=========================================================================
//  UNITCONVERSION.CC - part of
//                  OMNeT++/OMNEST
//           Discrete System Simulation in C++
//
//=========================================================================

/*--------------------------------------------------------------*
  Copyright (C) 1992-2005 Andras Varga

  This file is distributed WITHOUT ANY WARRANTY. See the file
  `license' for details on this and other legal matters.
*--------------------------------------------------------------*/

#include <assert.h>
#include "unitconversion.h"

UnitConversion::UnitDesc UnitConversion::unitTable[] = {
    { "d",   86400, "s",    "day" },
    { "h",    3600, "s",    "hour" },
    { "mi",     60, "s",    "minute" },
    { "s",       1, "s",    "second" },
    { "ms",   1e-3, "s",    "millisecond" },
    { "us",   1e-9, "s",    "microsecond" },
    { "ns",  1e-12, "s",    "nanosecond" },
    { "Tbps", 1e12, "bps",  "terabit/s" },
    { "Gbps",  1e9, "bps",  "gigabit/s" },
    { "Mbps",  1e6, "bps",  "megabit/s" },
    { "Kbps",  1e3, "bps",  "kilobit/s" },
    { "bps",     1, "bps",  "bit/s" },
    { "TB",   1e12, "B",    "terabyte" },
    { "GB",    1e9, "B",    "gigabyte" },
    { "MB",    1e6, "B",    "megabyte" },
    { "KB",    1e3, "B",    "kilobyte" },
    { "B",       1, "B",    "byte" },
    { "km",    1e3, "m",    "kilometer" },
    { "m",       1, "m",    "meter" },
    { "cm",   1e-2, "m",    "centimeter" },
    { "mm",   1e-3, "m",    "millimeter" },
    { "mW",   1e-3, "W",    "milliwatt" },
    { "W",       1, "W",    "watt" },
    { "GHz",   1e9, "Hz",   "gigaherz" },
    { "MHz",   1e6, "Hz",   "megaherz" },
    { "kHz",   1e3, "Hz",   "kiloherz" },
    { "Hz",      1, "Hz",   "herz" },
    { NULL,      0, NULL,   NULL }
};

UnitConversion::UnitDesc *UnitConversion::lookupUnit(const char *unit)
{
    for (int i=0; unitTable[i].unit; i++)
        if (!strcmp(unitTable[i].unit, unit))
            return unitTable+i;
    return NULL;
}

double UnitConversion::parseQuantity(const char *str, const char *expectedUnit)
{
__asm int 3;
    double result = 0;
    const char *unit = NULL;
    const char *s = str;

    UnitDesc *expectedUnitDesc = expectedUnit ? lookupUnit(expectedUnit) : NULL;
    bool expectedUnitIsBaseUnit = expectedUnitDesc ? expectedUnitDesc->mult==1 : false;

    while (*s!='\0')
    {
        // read number into num and skip it
        double num;
        int len;
        while (isspace(*s)) s++;
        if (0==sscanf(s, "%lf%n", &num, &len))
            break; // syntax error -- number expected
        s+=len;

        // number must be followed by a unit
        if (!isalpha(*s))
        {
            // if we came across a unit name earlier in this string, require one afterwards too
            if (unit)
                throw new Exception("syntax error parsing quantity '%s': missing unit", str);

            // if no unit follows, string must end after the number
            assert(result==0);
            result = num;
            break;
        }

        // extract unit
        const int MAXLEN=15;
        char tmpUnit[MAXLEN+1];
        int i;
        for (i=0; i<MAXLEN && isalpha(s[i]); i++)
            tmpUnit[i] = s[i];
        if (i==MAXLEN)
            throw new Exception("syntax error parsing quantity '%s': unit name too long", str);
        tmpUnit[i] = '\0';
        s += i;

        // look up and apply unit
        UnitDesc *tmpUnitDesc = lookupUnit(tmpUnit);
        if (!tmpUnitDesc || expectedUnitIsBaseUnit)
        {
            // Case A: no conversion. Either because unit is unknown, or the
            // expected unit is not a base unit (we won't convert to "ms" only "s"!)

            // unit must match expectedUnit
            if (expectedUnit && strcmp(expectedUnit, tmpUnit)!=0)
                throw new Exception("error in quantity '%s': supplied unit '%s' does not match expected unit '%s' "
                                    "(note that conversion is only performed into base units: s, m, Hz, B, bps, W)",
                                    str, tmpUnit, expectedUnit);

            // string must end here
            assert(result==0);
            result = num;
            break;
        }

        // known unit -- it must match expectedUnit and previous units in this string
        if (!unit)
        {
            unit = tmpUnitDesc->baseUnit;

            // meters given but seconds expected
            if (expectedUnit && strcmp(unit, expectedUnit)!=0)
                throw new Exception("error in quantity '%s': appears to be given in '%s' but '%s' is expected here",
                                    str, unit, expectedUnit);
        }
        else if (strcmp(tmpUnitDesc->baseUnit, unit)!=0)
        {
            // seconds and meters cannot be mixed within one string
            throw new Exception("error in quantity '%s': mismatch of units '%s' and '%s'",
                                str, unit, tmpUnit);
        }

        // convert kilometers to meters, etc
        result += tmpUnitDesc->mult * num;
    }

    // must be at the end of the input string
    while (isspace(*s)) s++;
    if (*s)
        throw new Exception("syntax error parsing quantity '%s'", str);

    // success
    return result;
}

std::string UnitConversion::formatQuantity(double d, const char *unit)
{
    printf("FIXME formatQuantity() to be implemented\n");
    return "FIXME";
}

