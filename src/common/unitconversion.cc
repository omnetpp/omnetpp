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
#include "opp_ctype.h"
#include "stringutil.h"
#include "unitconversion.h"

USING_NAMESPACE

UnitConversion::UnitDesc UnitConversion::unitTable[] = {
//FIXME accept longer names too ("byte", "bytes", "sec", "second", "seconds"...)
    { "d",   86400, "s",    "day" },
    { "h",    3600, "s",    "hour" },
    { "min",    60, "s",    "minute" },
    { "s",       1, "s",    "second" },
    { "ms",   1e-3, "s",    "millisecond" },
    { "us",   1e-6, "s",    "microsecond" },
    { "ns",   1e-9, "s",    "nanosecond" },
    { "Tbps", 1e12, "bps",  "terabit/sec" },
    { "Gbps",  1e9, "bps",  "gigabit/sec" },
    { "Mbps",  1e6, "bps",  "megabit/sec" },
    { "Kbps",  1e3, "bps",  "kilobit/sec" },
    { "bps",     1, "bps",  "bit/sec" },
    { "TB",   1e12, "B",    "terabyte" },
    { "GB",    1e9, "B",    "gigabyte" },
    { "MB",    1e6, "B",    "megabyte" },
    { "KB",    1e3, "B",    "kilobyte" },
    { "B",       1, "B",    "byte" },
    { "b",       1, "b",    "bit" },
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

bool UnitConversion::readNumber(const char *&s, double& number)
{
    while (opp_isspace(*s)) s++;

    int len;
    if (sscanf(s, "%lf%n", &number, &len) <= 0)
        return false; // no number read
    s+=len;

//FIXME use this code instead:
//    char *endp;
//    number = strtod(s, &endp);
//    if (s==endp)
//        return false; // no number read
//    if (errno==ERANGE)
//        throw opp_runtime_error("overflow or underflow during conversion of `%s'", s);
//    s = endp;

    while (opp_isspace(*s)) s++;
    return true; // OK
}

bool UnitConversion::readUnit(const char *&s, std::string& unit)
{
    unit = "";
    while (opp_isspace(*s)) s++;
    while (opp_isalpha(*s))
        unit.append(1, *s++);
    while (opp_isspace(*s)) s++;
    return !unit.empty();
}

double UnitConversion::parseQuantity(const char *str, const char *expectedUnit)
{
    std::string dummy;
    return doParseQuantity(str, expectedUnit, dummy);
}

double UnitConversion::parseQuantity(const char *str, std::string& outActualUnit)
{
    return doParseQuantity(str, NULL, outActualUnit);
}

double UnitConversion::doParseQuantity(const char *str, const char *expectedUnit, std::string& unit)
{
//FIXME only convert if there are several numbers (like "2h 5min")
    //FIXME warn if unit only differs from a "known" unit in uppercase/lowercase!!!!
    //FIXME handle "dimensionless" units too (expectedUnit=="-"; make it expectedUnit=="1" !!!)

    unit = "";
    const char *s = str;

    // read first number
    double num;
    if (!readNumber(s, num))
        throw opp_runtime_error("syntax error parsing quantity '%s': must begin with a number", str);

    // first, deal with special case: just a plain number without unit
    std::string tmpUnit;
    if (!readUnit(s, tmpUnit))
    {
        if (*s)
            throw opp_runtime_error("syntax error parsing quantity '%s': garbage after first number", str);
        return num;
    }

    // check first unit against expected unit
    UnitDesc *tmpUnitDesc = lookupUnit(tmpUnit.c_str());

    // we'll only want to perform conversion if expected unit is a base unit (s, m, bps, B, W, etc)
    UnitDesc *expectedUnitDesc = expectedUnit ? lookupUnit(expectedUnit) : NULL;
    bool performConversion = !expectedUnit ? tmpUnitDesc!=NULL : expectedUnitDesc ? expectedUnitDesc->mult==1 : false;

#define DESC(u) UnitConversion::unitDescription(u).c_str()

    // check it matches expected unit ("meters given but seconds expected")
    if (!performConversion && expectedUnit && tmpUnit!=expectedUnit)
        throw opp_runtime_error("error in quantity '%s': supplied unit %s does not match expected unit %s "
                            "(note that conversion is only performed into base units: s, m, Hz, B, bps, W)",
                            str, DESC(tmpUnit.c_str()), DESC(expectedUnit));

    if (performConversion && expectedUnit && (!tmpUnitDesc || std::string(tmpUnitDesc->baseUnit)!=expectedUnit))
        throw opp_runtime_error("error in quantity '%s': supplied unit %s does not match expected unit %s",
                            str, DESC(tmpUnit.c_str()), DESC(expectedUnit));
    unit = performConversion ? tmpUnitDesc->baseUnit : tmpUnit;

    double result = performConversion ? tmpUnitDesc->mult * num : num;

    // now process the rest: [<number> <unit>]*
    while (*s)
    {
        // read number
        double num;
        if (!readNumber(s, num))
            break;

        // read unit
        if (!readUnit(s, tmpUnit))
            throw opp_runtime_error("syntax error parsing quantity '%s': missing unit", str);

        // check unit
        UnitDesc *tmpUnitDesc = lookupUnit(tmpUnit.c_str());
        if (performConversion ? (!tmpUnitDesc || unit!=tmpUnitDesc->baseUnit) : unit!=tmpUnit)
            throw opp_runtime_error("error in quantity '%s': unit %s does not match %s",
                                str, DESC(tmpUnit.c_str()), DESC(unit.c_str()));

        // convert kilometers to meters, etc
        result += performConversion ? tmpUnitDesc->mult * num : num;
    }

    // must be at the end of the input string
    if (*s)
        throw opp_runtime_error("syntax error parsing quantity '%s'", str);

    // success
    return result;
}

std::string UnitConversion::formatQuantity(double d, const char *unit)
{
    printf("FIXME formatQuantity() to be implemented\n");
    return "FIXME";
}

std::string UnitConversion::unitDescription(const char *unit)
{
    UnitDesc *desc = lookupUnit(unit);
    std::string result = std::string("'")+unit+"'";
    if (desc)
        result += std::string(" (") + desc->longName + ")";
    return result;
}

double UnitConversion::convertUnit(double d, const char *unit, const char *targetUnit)
{
    // if units are the same (or there are no units), no conversion is needed
    if (unit==targetUnit || opp_strcmp(unit, targetUnit)==0)
        return d;

    // if only one unit is given, that's an error
    if (unit==NULL || targetUnit==NULL)
        throw opp_runtime_error("value is given in wrong units: expected %s and got %s",
                (opp_isempty(targetUnit) ? "none" : unitDescription(targetUnit).c_str()),
                (opp_isempty(unit) ? "none" : unitDescription(unit).c_str()));

    // we'll need to convert
    UnitDesc *unitDesc = lookupUnit(unit);
    UnitDesc *targetUnitDesc = lookupUnit(targetUnit);
    if (unitDesc==NULL || targetUnitDesc==NULL || strcmp(unitDesc->baseUnit, targetUnitDesc->baseUnit)!=0)
        throw opp_runtime_error("value is given in incompatible units: expected %s and got %s",
                (opp_isempty(targetUnit) ? "none" : unitDescription(targetUnit).c_str()),
                (opp_isempty(unit) ? "none" : unitDescription(unit).c_str()));
    return d * unitDesc->mult / targetUnitDesc->mult;
}

