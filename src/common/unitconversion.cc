//=========================================================================
//  UNITCONVERSION.CC - part of
//                  OMNeT++/OMNEST
//           Discrete System Simulation in C++
//
//  Author: Andras Varga
//
//=========================================================================

/*--------------------------------------------------------------*
  Copyright (C) 2006-2017 OpenSim Ltd.

  This file is distributed WITHOUT ANY WARRANTY. See the file
  `license' for details on this and other legal matters.
*--------------------------------------------------------------*/

#include <cstdio>
#include <cassert>
#include <clocale>
#include <cstdlib>
#include <cerrno>
#include "omnetpp/platdep/platmisc.h"  // strcasecmp
#include "opp_ctype.h"
#include "stringutil.h"
#include "unitconversion.h"

namespace omnetpp {
namespace common {

const double K = 1024.0;

UnitConversion::UnitDesc UnitConversion::unitTable[] = {  // note: imperial units (mile,foot,yard,etc.) intentionally left out
    { "s",       1, "s",    "second" },
    { "d",   86400, "s",    "day" },
    { "h",    3600, "s",    "hour" },
    { "min",    60, "s",    "minute" }, // "m" is meter
    { "ms",   1e-3, "s",    "millisecond" },
    { "us",   1e-6, "s",    "microsecond" },
    { "ns",   1e-9, "s",    "nanosecond" },
    { "ps",  1e-12, "s",    "picosecond" },
    { "fs",  1e-15, "s",    "femtosecond" },
    { "as",  1e-18, "s",    "attosecond" },
    { "bps",     1, "bps",  "bit/sec" },
    { "kbps",  1e3, "bps",  "kilobit/sec" },
    { "Mbps",  1e6, "bps",  "megabit/sec" },
    { "Gbps",  1e9, "bps",  "gigabit/sec" },
    { "Tbps", 1e12, "bps",  "terabit/sec" },
    { "B",       1, "B",    "byte" },
    { "KiB",     K, "B",    "kibibyte" },
    { "MiB",   K*K, "B",    "mebibyte" },
    { "GiB", K*K*K, "B",    "gibibyte" },
    { "TiB",K*K*K*K,"B",    "tebibyte" },
    { "kB",    1e3, "B",    "kilobyte" },
    { "MB",    1e6, "B",    "megabyte" },
    { "GB",    1e9, "B",    "gigabyte" },
    { "TB",   1e12, "B",    "terabyte" },
    { "b",       1, "b",    "bit" },
    { "m",       1, "m",    "meter" },
    { "mm",   1e-3, "m",    "millimeter" },
    { "cm",   1e-2, "m",    "centimeter" },
    { "km",    1e3, "m",    "kilometer" },
    { "W",       1, "W",    "watt" },
    { "mW",   1e-3, "W",    "milliwatt" },
    { "kW",    1e3, "W",    "kilowatt" },
    { "MW",   1e6, "MW",    "megawatt" },
    { "GW",   1e9, "MW",    "gigawatt" },
    { "Hz",      1, "Hz",   "hertz" },
    { "kHz",   1e3, "Hz",   "kilohertz" },
    { "MHz",   1e6, "Hz",   "megahertz" },
    { "GHz",   1e9, "Hz",   "gigahertz" },
    { "kg",      1, "kg",   "kilogram" },
    { "g",    1e-3, "kg",   "gram" },
    { "J",       1, "J",    "joule" },
    { "kJ",    1e3, "J",    "kilojoule" },
    { "MJ",    1e6, "J",    "megajoule" },
    { "V",       1, "V",    "volt" },
    { "kV",    1e3, "V",    "kilovolt" },
    { "mV",   1e-3, "V",    "millivolt" },
    { "A",       1, "A",    "ampere" },
    { "mA",   1e-3, "A",    "milliampere" },
    { "uA",   1e-6, "A",    "microampere" },
    { "mps",     1, "mps",  "meter/sec" },
    { "kmph",1/3.6, "mps",  "kilometer/hour" },
    { "C",       1, "C",    "coulomb"},
    { "As",      1, "C",    "ampere-second"},
    { "mAs",  1e-3, "C",    "milliampere-second"},
    { "Ah",   3600, "C",    "ampere-hour"},
    { "mAh",   3.6, "C",    "milliampere-hour"},
    // this many should be enough
    { nullptr,      0, nullptr,   nullptr }
};

UnitConversion::UnitDesc *UnitConversion::lookupUnit(const char *unit)
{
    // short name ("Hz", "mW")
    for (int i = 0; unitTable[i].unit; i++)
        if (!strcmp(unitTable[i].unit, unit))
            return unitTable+i;


    // long name, case insensitive ("herz", "milliwatt")
    for (int i = 0; unitTable[i].unit; i++)
        if (!strcasecmp(unitTable[i].longName, unit))
            return unitTable+i;


    // long name in plural, case insensitive ("milliwatts")
    if (unit[strlen(unit)-1] == 's') {
        std::string tmp = std::string(unit, strlen(unit)-1);
        for (int i = 0; unitTable[i].unit; i++)
            if (!strcasecmp(unitTable[i].longName, tmp.c_str()))
                return unitTable+i;

    }
    return nullptr;
}

bool UnitConversion::readNumber(const char *& s, double& number)
{
    while (opp_isspace(*s))
        s++;

    char *endp;
    setlocale(LC_NUMERIC, "C");
    errno = 0;
    number = strtod(s, &endp);
    if (s == endp)
        return false;  // no number read
    if (errno == ERANGE)
        throw opp_runtime_error("Overflow or underflow during conversion of '%s'", s);
    s = endp;

    while (opp_isspace(*s))
        s++;
    return true;  // OK
}

bool UnitConversion::readUnit(const char *& s, std::string& unit)
{
    unit = "";
    while (opp_isspace(*s))
        s++;
    while (opp_isalpha(*s))
        unit.append(1, *s++);
    while (opp_isspace(*s))
        s++;
    return !unit.empty();
}

double UnitConversion::parseQuantity(const char *str, const char *expectedUnit)
{
    std::string unit;
    double d = parseQuantity(str, unit);
    return convertUnit(d, unit.c_str(), expectedUnit);
}

double UnitConversion::parseQuantity(const char *str, std::string& unit)
{
    double result = 0;
    unit = "";
    const char *s = str;

    // read first number and unit
    if (!readNumber(s, result))
        throw opp_runtime_error("Syntax error parsing quantity '%s': Must begin with a number", str);
    if (!readUnit(s, unit)) {
        // special case: plain number without unit
        if (*s)
            throw opp_runtime_error("Syntax error parsing quantity '%s': Garbage after first number", str);
        return result;
    }

    // now process the rest: [<number> <unit>]*
    while (*s) {
        // read number and unit
        double d;
        if (!readNumber(s, d))
            break;
        std::string tmpUnit;
        if (!readUnit(s, tmpUnit))
            throw opp_runtime_error("Syntax error parsing quantity '%s': Missing unit", str);

        // check unit
        double factor = getConversionFactor(unit.c_str(), tmpUnit.c_str());
        if (factor == 0)
            throw opp_runtime_error("Error in quantity '%s': Unit %s does not match %s",
                    str, getUnitDescription(tmpUnit.c_str()).c_str(), getUnitDescription(unit.c_str()).c_str());

        // do the conversion
        result = result * factor + d;
        unit = tmpUnit;
    }

    // must be at the end of the input string
    if (*s)
        throw opp_runtime_error("Syntax error parsing quantity '%s'", str);

    // success
    return result;
}

std::string UnitConversion::formatQuantity(double d, const char *unit)
{
    return opp_stringf("%g%s", d, opp_nulltoempty(unit));
}

std::string UnitConversion::getUnitDescription(const char *unit)
{
    UnitDesc *desc = lookupUnit(unit);
    std::string result = std::string("'")+unit+"'";
    if (desc)
        result += std::string(" (") + desc->longName + ")";
    return result;
}

double UnitConversion::getConversionFactor(const char *unit, const char *targetUnit)
{
    // if there are no units or if units are the same, no conversion is needed
    if (unit == targetUnit || opp_strcmp(unit, targetUnit) == 0)
        return 1.0;

    // if only one unit is given, that's an error
    if (opp_isempty(unit) || opp_isempty(targetUnit))
        return 0;  // cannot convert

    // we'll need to convert
    UnitDesc *unitDesc = lookupUnit(unit);
    UnitDesc *targetUnitDesc = lookupUnit(targetUnit);
    if (unitDesc == nullptr || targetUnitDesc == nullptr || strcmp(unitDesc->baseUnit, targetUnitDesc->baseUnit) != 0)
        return 0;  // cannot convert

    // the solution
    return unitDesc->mult / targetUnitDesc->mult;
}

double UnitConversion::convertUnit(double d, const char *unit, const char *targetUnit)
{
    if (d == 0 && opp_isempty(unit))
        return 0;  // accept 0 without unit

    double factor = getConversionFactor(unit, targetUnit);
    if (factor == 0) {
        throw opp_runtime_error("Cannot convert unit %s to %s",
                (opp_isempty(unit) ? "none" : getUnitDescription(unit).c_str()),
                (opp_isempty(targetUnit) ? "none" : getUnitDescription(targetUnit).c_str()));
    }
    return factor * d;
}

const char *UnitConversion::getLongName(const char *unit)
{
    UnitDesc *unitDesc = lookupUnit(unit);
    return unitDesc ? unitDesc->longName : nullptr;
}

const char *UnitConversion::getBaseUnit(const char *unit)
{
    UnitDesc *unitDesc = lookupUnit(unit);
    return unitDesc ? unitDesc->baseUnit : nullptr;
}

std::vector<const char *> UnitConversion::getAllUnits()
{
    std::vector<const char *> result;
    for (int i = 0; unitTable[i].unit; i++)
        result.push_back(unitTable[i].unit);
    return result;
}

}  // namespace common
}  // namespace omnetpp

