//=========================================================================
//  UNITCONVERSION.CC - part of
//                  OMNeT++/OMNEST
//           Discrete System Simulation in C++
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
#include <algorithm>
#include "omnetpp/platdep/platmisc.h"  // strcasecmp
#include "opp_ctype.h"
#include "stringutil.h"
#include "unitconversion.h"
#include "stringtokenizer.h"

using namespace std;

namespace omnetpp {
namespace common {

const double K = 1024.0;

UnitConversion::UnitDesc UnitConversion::unitTable[] = {  // note: imperial units (mile,foot,yard,etc.) intentionally left out
#define _ LINEAR
    { "d",   86400, _, "s",    "day",         "s ms us ns ps fs as" },
    { "h",    3600, _, "s",    "hour",        "s ms us ns ps fs as" },
    { "min",    60, _, "s",    "minute",      "s ms us ns ps fs as" }, // "m" is reserved for meter
    { "s",       1, _, "s",    "second",      "s ms us ns ps fs as" },
    { "ms",   1e-3, _, "s",    "millisecond", "s ms us ns ps fs as" },
    { "us",   1e-6, _, "s",    "microsecond", "s ms us ns ps fs as" },
    { "ns",   1e-9, _, "s",    "nanosecond",  "s ms us ns ps fs as" },
    { "ps",  1e-12, _, "s",    "picosecond",  "s ms us ns ps fs as" },
    { "fs",  1e-15, _, "s",    "femtosecond", "s ms us ns ps fs as" },
    { "as",  1e-18, _, "s",    "attosecond",  "s ms us ns ps fs as" },
    { "bps",     1, _, "bps",  "bit/sec",     "bps kbps Mbps Gbps Tbps" },
    { "kbps",  1e3, _, "bps",  "kilobit/sec", "bps kbps Mbps Gbps Tbps" },
    { "Mbps",  1e6, _, "bps",  "megabit/sec", "bps kbps Mbps Gbps Tbps" },
    { "Gbps",  1e9, _, "bps",  "gigabit/sec", "bps kbps Mbps Gbps Tbps" },
    { "Tbps", 1e12, _, "bps",  "terabit/sec", "bps kbps Mbps Gbps Tbps" },
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
    { "deg",M_PI/180,_,"rad",  "degree"  },
    { "m",       1, _, "m",    "meter",      "km m mm um nm" },
    { "cm",   1e-2, _, "m",    "centimeter", "km m mm um nm" },
    { "mm",   1e-3, _, "m",    "millimeter", "km m mm um nm" },
    { "um",   1e-6, _, "m",    "micrometer", "km m mm um nm" },
    { "nm",   1e-9, _, "m",    "nanometer",  "km m mm um nm" },
    { "km",    1e3, _, "m",    "kilometer",  "km m mm um nm" },
    { "W",       1, _, "W",    "watt",       "GW MW kW W mW uW nW pW fW" },
    { "mW",   1e-3, _, "W",    "milliwatt",  "GW MW kW W mW uW nW pW fW" },
    { "uW",   1e-6, _, "W",    "microwatt",  "GW MW kW W mW uW nW pW fW" },
    { "nW",   1e-9, _, "W",    "nanowatt",   "GW MW kW W mW uW nW pW fW" },
    { "pW",  1e-12, _, "W",    "picowatt",   "GW MW kW W mW uW nW pW fW" },
    { "fW",  1e-15, _, "W",    "femtowatt",  "GW MW kW W mW uW nW pW fW" },
    { "kW",    1e3, _, "W",    "kilowatt",   "GW MW kW W mW uW nW pW fW" },
    { "MW",    1e6, _, "W",    "megawatt",   "GW MW kW W mW uW nW pW fW" },
    { "GW",    1e9, _, "W",    "gigawatt",   "GW MW kW W mW uW nW pW fW" },
    { "Hz",      1, _, "Hz",   "hertz",      "Hz kHz MHz GHz THz" },
    { "kHz",   1e3, _, "Hz",   "kilohertz",  "Hz kHz MHz GHz THz" },
    { "MHz",   1e6, _, "Hz",   "megahertz",  "Hz kHz MHz GHz THz" },
    { "GHz",   1e9, _, "Hz",   "gigahertz",  "Hz kHz MHz GHz THz" },
    { "THz",  1e12, _, "Hz",   "terahertz",  "Hz kHz MHz GHz THz" },
    { "kg",      1, _, "kg",   "kilogram",   "kg g" },
    { "g",    1e-3, _, "kg",   "gram",       "kg g" },
    { "K",      1, _,  "K",    "kelvin" },
    { "J",       1, _, "J",    "joule",         "J kJ MJ" },
    { "kJ",    1e3, _, "J",    "kilojoule",     "J kJ MJ" },
    { "MJ",    1e6, _, "J",    "megajoule",     "J kJ MJ" },
    { "Ws",      1, _, "J",    "watt-second",   "Ws" },
    { "Wh",   3600, _, "J",    "watt-hour",     "Wh kWh MWh" },
    { "kWh",3600e3, _, "J",    "kilowatt-hour", "Wh kWh MWh" },
    { "MWh",3600e6, _, "J",    "megawatt-hour", "Wh kWh MWh" },
    { "V",       1, _, "V",    "volt",          "V kV mV" },
    { "kV",    1e3, _, "V",    "kilovolt",      "V kV mV" },
    { "mV",   1e-3, _, "V",    "millivolt",     "V kV mV" },
    { "A",       1, _, "A",    "ampere",        "A mA uA" },
    { "mA",   1e-3, _, "A",    "milliampere",   "A mA uA" },
    { "uA",   1e-6, _, "A",    "microampere",   "A mA uA" },
    { "Ohm",     1, _, "Ohm",  "ohm",       "Ohm mOhm kOhm MOhm" },
    { "mOhm", 1e-3, _, "Ohm",  "milliohm",  "Ohm mOhm kOhm MOhm" },
    { "kOhm",  1e3, _, "Ohm",  "kiloohm",   "Ohm mOhm kOhm MOhm" },
    { "MOhm",  1e6, _, "Ohm",  "megaohm",   "Ohm mOhm kOhm MOhm" },
    { "mps",     1, _, "mps",  "meter/sec" },
    { "kmps",  1e3, _, "mps",  "kilometer/sec" },
    { "kmph",1/3.6, _, "mps",  "kilometer/hour" },
    { "C",       1, _, "As",   "coulomb" },
    { "As",      1, _, "As",   "ampere-second",      "As mAs" },
    { "mAs",  1e-3, _, "As",   "milliampere-second", "As mAs" },
    { "Ah",   3600, _, "As",   "ampere-hour",        "Ah mAh" },
    { "mAh",   3.6, _, "As",   "milliampere-hour",   "Ah mAh" },
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

UnitConversion::UnitDesc *UnitConversion::hashTable[HASHTABLESIZE];
int UnitConversion::numCollisions = 0;

bool UnitConversion::initCalled = false;

bool UnitConversion::matches(UnitDesc *desc, const char *unit)
{
    // short name
    if (strcmp(desc->unit, unit) == 0)
        return true;
    // if unit is at least 3 chars, look up by long name, case insensitive ("herz", "milliwatt")
    if (unit[1] && unit[2] && strcasecmp(desc->longName, unit) == 0)
        return true;
    return false;
}

inline unsigned lc(char c) {return (unsigned)c | 0x20;} // assuming ASCII letters

inline unsigned UnitConversion::hashCode(const char *unit)
{
    unsigned result = lc(unit[0]);
    if (!unit[1]) return result;
    result = result*65 + lc(unit[1]);
    if (!unit[2]) return result;
    result = result*65 + lc(unit[2]);
    for (const char *s = unit+3; *s; s++)
        result = result*65 + lc(*s);
    return result;
}

UnitConversion::UnitDesc *UnitConversion::lookupUnit(const char *unit)
{
    // fill hash table on first call
    if (!initCalled)
        init();

    if (!*unit)
        return nullptr; // empty string is not a unit

    // hash table lookup
    unsigned hash = hashCode(unit);
    for (unsigned pos = hash & (HASHTABLESIZE-1); hashTable[pos]; pos = (pos+1) & (HASHTABLESIZE-1))
        if (matches(hashTable[pos], unit))
            return hashTable[pos];

    // not found, maybe it's a plural form of long name, retry by slicing off the trailing "s"
    int len = strlen(unit);
    if (len >= 4 && unit[len-1] == 's')
        return lookupUnit(std::string(unit, len-1).c_str());

    return nullptr;
}

void UnitConversion::insert(const char *key, UnitDesc *desc)
{
    int localCollisions = 0;
    unsigned hash = hashCode(key);
    unsigned pos;
    for (pos = hash & (HASHTABLESIZE-1); hashTable[pos]; pos = (pos+1) & (HASHTABLESIZE-1))
        localCollisions++;
    hashTable[pos] = desc;
    numCollisions += localCollisions;
    Assert(localCollisions <= 2); // usually 0, rarely 1 collision observed at the time of writing
}

void UnitConversion::init()
{
    initCalled = true;
    fillHashtable();
    fillBaseUnitDescs();
}

void UnitConversion::fillHashtable()
{
    for (UnitDesc *p = unitTable; p->unit; p++) {
        insert(p->unit, p);
        insert(p->longName, p);
    }
    Assert(numCollisions <= 25); // 21 collisions observed at the time of writing
}

void UnitConversion::fillBaseUnitDescs()
{
    for (UnitDesc *p = unitTable; p->unit; p++) {
        p->baseUnitDesc = lookupUnit(p->baseUnit);
        for (std::string unit : opp_splitandtrim(p->bestUnitCandidatesStr))
            p->bestUnitCandidates.push_back(lookupUnit(unit.c_str()));
    }
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

    bool minus = false;
    if (result < 0) {
        minus = true;
        result = -result;
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

        // check number
        if (d < 0)
            throw opp_runtime_error("Syntax error parsing quantity '%s': Minus sign only accepted at the front", str);

        // check unit
        if (!isLinearUnit(unit.c_str()) || !isLinearUnit(tmpUnit.c_str()))
            throw opp_runtime_error("Error in quantity '%s': Concatenated quantity literals are not supported with non-linear units like dB", str);
        double factor = getConversionFactor(unit.c_str(), tmpUnit.c_str());
        if (factor == 0)
            throw opp_runtime_error("Error in quantity '%s': Unit %s does not match %s",
                    str, getUnitDescription(tmpUnit.c_str()).c_str(), getUnitDescription(unit.c_str()).c_str());
        if (factor <= 1)
            throw opp_runtime_error("Error in quantity '%s': Units must be in strictly decreasing order of magnitude", str); // to reduce chance of confusing a compound quantity with a list

        // do the conversion
        result = result * factor + d;
        unit = tmpUnit;
    }

    if (minus)
        result = -result;

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
    if (unitDesc->baseUnitDesc == targetUnitDesc && unitDesc->mapping == LINEAR)
        return unitDesc->mult;
    if (unitDesc == targetUnitDesc->baseUnitDesc && targetUnitDesc->mapping == LINEAR)
        return 1.0 / targetUnitDesc->mult;

    // convert unit to the base, and try again
    if (unitDesc != unitDesc->baseUnitDesc && unitDesc->mapping == LINEAR) {
        UnitDesc *baseUnitDesc = lookupUnit(unitDesc->baseUnit);
        return unitDesc->mult * tryGetConversionFactor(baseUnitDesc, targetUnitDesc);
    }

    // try converting via the target unit's base
    if (targetUnitDesc != targetUnitDesc->baseUnitDesc && targetUnitDesc->mapping == LINEAR) {
        UnitDesc *targetBaseDesc = targetUnitDesc->baseUnitDesc;
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
    if (unitDesc->baseUnitDesc == targetUnitDesc)
        return convertToBase(value, unitDesc);
    if (unitDesc == targetUnitDesc->baseUnitDesc)
        return convertFromBase(value, targetUnitDesc);

    // convert unit to the base, and try again
    if (unitDesc != unitDesc->baseUnitDesc) {
        UnitDesc *baseUnitDesc = unitDesc->baseUnitDesc;
        return tryConvert(convertToBase(value, unitDesc), baseUnitDesc, targetUnitDesc);
    }

    // try converting via the target unit's base
    if (targetUnitDesc->unit != targetUnitDesc->baseUnit) {
        UnitDesc *targetBaseDesc = targetUnitDesc->baseUnitDesc;
        return convertFromBase(tryConvert(value, unitDesc, targetBaseDesc), targetUnitDesc);
    }

    return NaN;
}

const char *UnitConversion::getBestUnit(double d, const char *unit)
{
    // We do not touch the unit (return the original unit) in a number of cases:
    // if value is zero, infinite or NaN; if we don't know about the unit.
    UnitDesc *unitDesc = lookupUnit(unit);
    if (d == 0 || !std::isfinite(d) || unitDesc == nullptr)
        return unit;

    auto& candidates = unitDesc->bestUnitCandidates;
    if (candidates.empty())
        return unit;

    // pick best one
    d = fabs(d);
    double valueInBaseUnit = d * unitDesc->mult;
    auto better = [=](UnitDesc *a, UnitDesc *b) {
        // return true if "a" is better than "b", false otherwise
        bool greaterThanOneInUnitA = (valueInBaseUnit / a->mult) >= 1.0;
        bool greaterThanOneInUnitB = (valueInBaseUnit / b->mult) >= 1.0;
        if (greaterThanOneInUnitA && greaterThanOneInUnitB)
            return a->mult > b->mult; // prefer bigger unit (results in smaller value)
        else if (!greaterThanOneInUnitA && !greaterThanOneInUnitB)
            return a->mult < b->mult; // prefer smaller unit (value will be < 1.0 but to closer to 1.0)
        else
            return greaterThanOneInUnitA; // prefer the one that results in >= 1.0 value
    };
    auto it = std::min_element(candidates.begin(), candidates.end(), better);
    UnitDesc *bestDesc = *it;

    // give the original unit a chance to win too (so we don't pointlessly convert e.g. 1cm to 10mm);
    // also, do not change the unit if it wouldn't change the value (e.g. don't change 1As to 1C)
    if (bestDesc != unitDesc && (better(unitDesc, bestDesc) || bestDesc->mult == unitDesc->mult))
        bestDesc = unitDesc;

    return bestDesc->unit;
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

