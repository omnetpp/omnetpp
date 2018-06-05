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
#include <cmath>
#include "omnetpp/platdep/platmisc.h"  // strcasecmp
#include "opp_ctype.h"
#include "stringutil.h"
#include "unitconversion.h"

using namespace std;

namespace omnetpp {
namespace common {

const double K = 1024.0;

UnitConversion::UnitDesc UnitConversion::unitTable[] = {  // note: imperial units (mile,foot,yard,etc.) intentionally left out
#define _ LINEAR
    { "s",       1, _, "s",    "second" },
    { "d",   86400, _, "s",    "day" },
    { "h",    3600, _, "s",    "hour" },
    { "min",    60, _, "s",    "minute" }, // "m" is reserved for meter
    { "ms",   1e-3, _, "s",    "millisecond" },
    { "us",   1e-6, _, "s",    "microsecond" },
    { "ns",   1e-9, _, "s",    "nanosecond" },
    { "ps",  1e-12, _, "s",    "picosecond" },
    { "fs",  1e-15, _, "s",    "femtosecond" },
    { "as",  1e-18, _, "s",    "attosecond" },
    { "bps",     1, _, "bps",  "bit/sec" },
    { "kbps",  1e3, _, "bps",  "kilobit/sec" },
    { "Mbps",  1e6, _, "bps",  "megabit/sec" },
    { "Gbps",  1e9, _, "bps",  "gigabit/sec" },
    { "Tbps", 1e12, _, "bps",  "terabit/sec" },
    { "B",       8, _, "b",    "byte" },
    { "KiB",     K, _, "B",    "kibibyte" },
    { "MiB",   K*K, _, "B",    "mebibyte" },
    { "GiB", K*K*K, _, "B",    "gibibyte" },
    { "TiB",K*K*K*K,_, "B",    "tebibyte" },
    { "kB",    1e3, _, "B",    "kilobyte" },
    { "MB",    1e6, _, "B",    "megabyte" },
    { "GB",    1e9, _, "B",    "gigabyte" },
    { "TB",   1e12, _, "B",    "terabyte" },
    { "b",       1, _, "b",    "bit" },
    { "Kib",     K, _, "b",    "kibibit" },
    { "Mib",   K*K, _, "b",    "mebibit" },
    { "Gib", K*K*K, _, "b",    "gibibit" },
    { "Tib",K*K*K*K,_, "b",    "tebibit" },
    { "kb",    1e3, _, "b",    "kilobit" },
    { "Mb",    1e6, _, "b",    "megabit" },
    { "Gb",    1e9, _, "b",    "gigabit" },
    { "Tb",   1e12, _, "b",    "terabit" },
    { "rad",   1,   _, "rad",  "radian"  },
    { "deg",M_PI/180,_,"rad",  "degree" },
    { "m",       1, _, "m",    "meter" },
    { "mm",   1e-3, _, "m",    "millimeter" },
    { "cm",   1e-2, _, "m",    "centimeter" },
    { "km",    1e3, _, "m",    "kilometer" },
    { "W",       1, _, "W",    "watt" },
    { "mW",   1e-3, _, "W",    "milliwatt" },
    { "uW",   1e-6, _, "W",    "microwatt" },
    { "nW",   1e-9, _, "W",    "nanowatt" },
    { "pW",  1e-12, _, "W",    "picowatt" },
    { "fW",  1e-15, _, "W",    "femtowatt" },
    { "kW",    1e3, _, "W",    "kilowatt" },
    { "MW",    1e6, _, "W",    "megawatt" },
    { "GW",    1e9, _, "W",    "gigawatt" },
    { "Hz",      1, _, "Hz",   "hertz" },
    { "kHz",   1e3, _, "Hz",   "kilohertz" },
    { "MHz",   1e6, _, "Hz",   "megahertz" },
    { "GHz",   1e9, _, "Hz",   "gigahertz" },
    { "THz",  1e12, _, "Hz",   "terahertz" },
    { "kg",      1, _, "kg",   "kilogram" },
    { "g",    1e-3, _, "kg",   "gram" },
    { "K",      1, _,  "K",    "kelvin" },
    { "J",       1, _, "J",    "joule" },
    { "kJ",    1e3, _, "J",    "kilojoule" },
    { "MJ",    1e6, _, "J",    "megajoule" },
    { "V",       1, _, "V",    "volt" },
    { "kV",    1e3, _, "V",    "kilovolt" },
    { "mV",   1e-3, _, "V",    "millivolt" },
    { "A",       1, _, "A",    "ampere" },
    { "mA",   1e-3, _, "A",    "milliampere" },
    { "uA",   1e-6, _, "A",    "microampere" },
    { "Ohm",     1, _, "Ohm",  "ohm"},
    { "mOhm", 1e-3, _, "Ohm",  "milliohm"},
    { "kOhm",  1e3, _, "Ohm",  "kiloohm"},
    { "MOhm",  1e6, _, "Ohm",  "megaohm"},
    { "mps",     1, _, "mps",  "meter/sec" },
    { "kmps",  1e3, _, "mps",  "kilometer/sec" },
    { "kmph",1/3.6, _, "mps",  "kilometer/hour" },
    { "C",       1, _, "C",    "coulomb" },
    { "As",      1, _, "C",    "ampere-second" },
    { "mAs",  1e-3, _, "C",    "milliampere-second" },
    { "Ah",   3600, _, "C",    "ampere-hour" },
    { "mAh",   3.6, _, "C",    "milliampere-hour" },
    { "ratio",   1, _, "ratio","ratio" },
    { "pct",  0.01, _, "ratio","percent" },
    // logarithmic
    { "dBW",    10, LOG10, "W",  "decibel-watt" },  // y = 10*log10(x)
    { "dBm",    10, LOG10, "mW", "decibel-milliwatt" },
    { "dBmW",   10, LOG10, "mW", "decibel-milliwatt" },
    { "dBV",    20, LOG10, "V",  "decibel-volt" },
    { "dBmV",   20, LOG10, "mV", "decibel-millivolt" },
    { "dBA",    20, LOG10, "A",  "decibel-ampere" },
    { "dBmA",   20, LOG10, "mA", "decibel-milliampere" },
    { "dB",     20, LOG10, "ratio", "decibel" },
    { nullptr,  0,  _, nullptr, nullptr }
#undef _
};

inline bool equal(const char *a, const char *b) {return strcmp(a,b)==0;}

UnitConversion::UnitDesc *UnitConversion::lookupUnit(const char *unit)
{
    // short name ("Hz", "mW")
    for (int i = 0; unitTable[i].unit; i++)
        if (equal(unitTable[i].unit, unit))
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
    const char *str = s;

    while (opp_isspace(*s))
        s++;

    char *endp;
    setlocale(LC_NUMERIC, "C");
    errno = 0;
    number = strtod(s, &endp);
    if (s == endp)
        return false;  // no number read
    if (errno == ERANGE)
        throw opp_runtime_error("Overflow or underflow during conversion of '%s'", str);
    s = endp;

    if (opp_isalpha(*(s-1)) && opp_isalpha(*s))
        throw opp_runtime_error("Syntax error parsing quantity '%s': Space required after nan/inf", str);

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
    double value = parseQuantity(str, unit);
    return convertUnit(value, unit.c_str(), expectedUnit);
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
        if (!isLinearUnit(unit.c_str()) || !isLinearUnit(tmpUnit.c_str()))
            throw opp_runtime_error("Error in quantity '%s': Concatenated quantity literals are not supported with non-linear units like dB", str);
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

std::string UnitConversion::formatQuantity(double value, const char *unit)
{
    return opp_stringf("%g%s", value, opp_nulltoempty(unit));
}

std::string UnitConversion::getUnitDescription(const char *unit)
{
    UnitDesc *desc = lookupUnit(unit);
    std::string result = std::string("'")+unit+"'";
    if (desc)
        result += std::string(" (") + desc->longName + ")";
    return result;
}

std::string UnitConversion::getConversionDescription(const char *unit)
{
    std::stringstream os;
    UnitDesc *unitDesc = lookupUnit(unit);
    switch (unitDesc->mapping) {
        case LINEAR: os << unitDesc->mult << unitDesc->baseUnit; break;
        case LOG10:  os << unitDesc->mult << "*log10(" << unitDesc->baseUnit << ")"; break;
    }
    return os.str();
}

double UnitConversion::getConversionFactor(const char *unit, const char *targetUnit)
{
    // if there are no units or if units are the same, no conversion is needed
    if (unit == targetUnit || opp_strcmp(unit, targetUnit) == 0)
        return 1.0;

    // if only one unit is given, that's an error
    if (opp_isempty(unit) || opp_isempty(targetUnit))
        return 0;  // cannot convert

    // look up both units
    UnitDesc *unitDesc = lookupUnit(unit);
    UnitDesc *targetUnitDesc = lookupUnit(targetUnit);
    if (unitDesc == nullptr || targetUnitDesc == nullptr)
        return 0;  // one of them is custom unit, cannot convert

    return tryGetConversionFactor(unitDesc, targetUnitDesc);
}

double UnitConversion::tryGetConversionFactor(UnitDesc *unitDesc, UnitDesc *targetUnitDesc)
{
    // if they are the same units, or one is the base unit of the other, we're done
    if (unitDesc == targetUnitDesc)
        return 1.0;
    if (equal(unitDesc->baseUnit, targetUnitDesc->unit) && unitDesc->mapping == LINEAR)
        return unitDesc->mult;
    if (equal(unitDesc->unit, targetUnitDesc->baseUnit) && targetUnitDesc->mapping == LINEAR)
        return 1.0 / targetUnitDesc->mult;
    
    // convert unit to the base, and try again
    if (!equal(unitDesc->unit, unitDesc->baseUnit) && unitDesc->mapping == LINEAR) {
        UnitDesc *baseUnitDesc = lookupUnit(unitDesc->baseUnit);
        return unitDesc->mult * tryGetConversionFactor(baseUnitDesc, targetUnitDesc);
    }

    // try converting via the target unit's base
    if (!equal(targetUnitDesc->unit, targetUnitDesc->baseUnit) && targetUnitDesc->mapping == LINEAR) {
        UnitDesc *targetBaseDesc = lookupUnit(targetUnitDesc->baseUnit);
        return tryGetConversionFactor(unitDesc, targetBaseDesc) / targetUnitDesc->mult;
    }
    
    return 0; // no conversion found
}

double UnitConversion::convertToBase(double value, UnitDesc *unitDesc)
{
    switch (unitDesc->mapping) {
        case LINEAR: return unitDesc->mult * value;
        case LOG10:  return pow(10.0, value / unitDesc->mult);
        default: throw opp_runtime_error("UnitConversion: invalid unit type");
    }
}

double UnitConversion::convertFromBase(double value, UnitDesc *unitDesc)
{
    switch (unitDesc->mapping) {
        case LINEAR: return value / unitDesc->mult;
        case LOG10:  return unitDesc->mult * log10(value);
        default: throw opp_runtime_error("UnitConversion: invalid unit type");
    }
}

double UnitConversion::convertUnit(double value, const char *unit, const char *targetUnit)
{
    // if there are no units or if units are the same, no conversion is needed
    if (unit == targetUnit || opp_strcmp(unit, targetUnit) == 0)
        return value;

    // allow missing unit for NaN (NaN is NaN in every unit)
    if (std::isnan(value) && opp_isempty(unit))
        return value;

    // if value is zero, unit may be omitted (if target unit is linear, e.g. not for dB) --TODO remove this in 6.0
    if (value == 0 && opp_isempty(unit) && !opp_isempty(targetUnit) && isLinearUnit(targetUnit))
        return 0;

    // if only one unit is given, that's an error
    if (opp_isempty(unit) || opp_isempty(targetUnit))
        cannotConvert(unit, targetUnit);

    // look up both units
    UnitDesc *unitDesc = lookupUnit(unit);
    UnitDesc *targetUnitDesc = lookupUnit(targetUnit);
    if (unitDesc == nullptr || targetUnitDesc == nullptr)
        cannotConvert(unit, targetUnit); // one of them is custom unit

    // convert
    double res = tryConvert(value, unitDesc, targetUnitDesc);
    if (std::isnan(res) && !std::isnan(value))
        cannotConvert(unit, targetUnit);
    return res;
}

void UnitConversion::cannotConvert(const char *unit, const char *targetUnit)
{
    throw opp_runtime_error("Cannot convert unit %s to %s",
            (opp_isempty(unit) ? "none" : getUnitDescription(unit).c_str()),
            (opp_isempty(targetUnit) ? "none" : getUnitDescription(targetUnit).c_str()));
}

double UnitConversion::tryConvert(double value, UnitDesc *unitDesc, UnitDesc *targetUnitDesc)
{
    // if they are the same units, or one is the base unit of the other, we're done
    if (unitDesc == targetUnitDesc)
        return value;
    if (equal(unitDesc->baseUnit, targetUnitDesc->unit))
        return convertToBase(value, unitDesc);
    if (equal(unitDesc->unit, targetUnitDesc->baseUnit))
        return convertFromBase(value, targetUnitDesc);
    
    // convert unit to the base, and try again
    if (!equal(unitDesc->unit, unitDesc->baseUnit)) {
        UnitDesc *baseUnitDesc = lookupUnit(unitDesc->baseUnit);
        return tryConvert(convertToBase(value, unitDesc), baseUnitDesc, targetUnitDesc);
    }

    // try converting via the target unit's base
    if (!equal(targetUnitDesc->unit, targetUnitDesc->baseUnit)) {
        UnitDesc *targetBaseDesc = lookupUnit(targetUnitDesc->baseUnit);
        return convertFromBase(tryConvert(value, unitDesc, targetBaseDesc), targetUnitDesc);
    }
    
    return NaN;
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

bool UnitConversion::isLinearUnit(const char *unit)
{
    UnitDesc *unitDesc = lookupUnit(unit);
    return unitDesc ? unitDesc->mapping == LINEAR : true;
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

