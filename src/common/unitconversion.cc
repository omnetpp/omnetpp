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
#include "stlutil.h"

using namespace std;

namespace omnetpp {
namespace common {

const bool debugScoring = false;

const double K = 1024.0;
const double K8 = 8192.0;

UnitConversion::Unit UnitConversion::unitTable[] = {  // note: imperial units (mile,foot,yard,etc.) intentionally left out
#define _ LINEAR
    { "d",   86400, _, "s",    "day",         "d h min s ms us ns ps fs as" },
    { "h",    3600, _, "s",    "hour",        "d h min s ms us ns ps fs as" },
    { "min",    60, _, "s",    "minute",      "d h min s ms us ns ps fs as" }, // "m" is reserved for meter
    { "s",       1, _, "s",    "second",      "d h min s ms us ns ps fs as" },
    { "ms",   1e-3, _, "s",    "millisecond", "d h min s ms us ns ps fs as" },
    { "us",   1e-6, _, "s",    "microsecond", "d h min s ms us ns ps fs as" },
    { "ns",   1e-9, _, "s",    "nanosecond",  "d h min s ms us ns ps fs as" },
    { "ps",  1e-12, _, "s",    "picosecond",  "d h min s ms us ns ps fs as" },
    { "fs",  1e-15, _, "s",    "femtosecond", "d h min s ms us ns ps fs as" },
    { "as",  1e-18, _, "s",    "attosecond",  "d h min s ms us ns ps fs as" },
    { "bps",     1, _, "bps",  "bit/sec",     "bps kbps Mbps Gbps Tbps" },
    { "kbps",  1e3, _, "bps",  "kilobit/sec", "bps kbps Mbps Gbps Tbps" },
    { "Mbps",  1e6, _, "bps",  "megabit/sec", "bps kbps Mbps Gbps Tbps" },
    { "Gbps",  1e9, _, "bps",  "gigabit/sec", "bps kbps Mbps Gbps Tbps" },
    { "Tbps", 1e12, _, "bps",  "terabit/sec", "bps kbps Mbps Gbps Tbps" },
    { "B",       8, _, "b",    "byte",        "B KiB MiB GiB TiB" },
    { "KiB",    K8, _, "b",    "kibibyte",    "B KiB MiB GiB TiB" },
    { "MiB",  K8*K, _, "b",    "mebibyte",    "B KiB MiB GiB TiB" },
    { "GiB",K8*K*K, _, "b",    "gibibyte",    "B KiB MiB GiB TiB" },
    { "TiB",K8*K*K*K,_,"b",    "tebibyte",    "B KiB MiB GiB TiB" },
    { "kB",    8e3, _, "b",    "kilobyte",    "B kB MB GB TB" },
    { "MB",    8e6, _, "b",    "megabyte",    "B kB MB GB TB" },
    { "GB",    8e9, _, "b",    "gigabyte",    "B kB MB GB TB" },
    { "TB",   8e12, _, "b",    "terabyte",    "B kB MB GB TB" },
    { "b",       1, _, "b",    "bit" },
    { "Kib",     K, _, "b",    "kibibit",     "b Kib Mib Gib Tib" },
    { "Mib",   K*K, _, "b",    "mebibit",     "b Kib Mib Gib Tib" },
    { "Gib", K*K*K, _, "b",    "gibibit",     "b Kib Mib Gib Tib" },
    { "Tib",K*K*K*K,_, "b",    "tebibit",     "b Kib Mib Gib Tib" },
    { "kb",    1e3, _, "b",    "kilobit",     "b kb Mb Gb Tb" },
    { "Mb",    1e6, _, "b",    "megabit",     "b kb Mb Gb Tb" },
    { "Gb",    1e9, _, "b",    "gigabit",     "b kb Mb Gb Tb" },
    { "Tb",   1e12, _, "b",    "terabit",     "b kb Mb Gb Tb" },
    { "rad",   1,   _, "rad",  "radian"  },
    { "deg",M_PI/180,_,"rad",  "degree"  },
    { "m",       1, _, "m",    "meter",      "km m mm um nm" },
    { "dm",   1e-1, _, "m",    "decimeter",  "km m mm um nm" },
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
    { "t",     1e3, _, "kg",   "tonne",      "kg g" },
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
    { "kmps",  1e3, _, "mps",  "kilometer/sec",  "mps" },
    { "kmph",1/3.6, _, "mps",  "kilometer/hour", "mps" },
    { "C",       1, _, "As",   "coulomb" },
    { "As",      1, _, "As",   "ampere-second",      "As mAs" },
    { "mAs",  1e-3, _, "As",   "milliampere-second", "As mAs" },
    { "Ah",   3600, _, "As",   "ampere-hour",        "Ah mAh" },
    { "mAh",   3.6, _, "As",   "milliampere-hour",   "Ah mAh" },
    { "x",       1, _, "x",    "times" },
    // logarithmic
    { "dBW",    10, LOG10, "W",  "decibel-watt",       "dBW dBmW" },  // y = 10*log10(x)
    { "dBm",    10, LOG10, "mW", "decibel-milliwatt",  "dBW dBmW" },
    { "dBmW",   10, LOG10, "mW", "decibel-milliwatt",  "dBW dBmW" },
    { "dBV",    20, LOG10, "V",  "decibel-volt",       "dBV dBmV" },
    { "dBmV",   20, LOG10, "mV", "decibel-millivolt",  "dBV dBmV" },
    { "dBA",    20, LOG10, "A",  "decibel-ampere",     "dBA dBmA" },
    { "dBmA",   20, LOG10, "mA", "decibel-milliampere","dBA dBmA" },
    { "dB",     20, LOG10, "x",  "decibel" },
    { nullptr,  0,  _, nullptr, nullptr }
#undef _
};

const UnitConversion::Unit *UnitConversion::hashTable[HASHTABLESIZE];
int UnitConversion::numCollisions = 0;

const UnitConversion::Unit *UnitConversion::bitsUnit;
const UnitConversion::Unit *UnitConversion::bytesUnit;
const UnitConversion::Unit *UnitConversion::secondsUnit;

static struct Initializer {
    Initializer() {UnitConversion::init();}
} dummy;

static const char *preferenceName(UnitConversion::Preference p)
{
    switch (p) {
        case UnitConversion::PREFER: return "PREFER";
        case UnitConversion::AVOID: return "AVOID";
        case UnitConversion::KEEP: return "KEEP";
        default: return "???";
    }
}
std::string UnitConversion::Options::str() const
{
    std::stringstream os;
    os << "useBaseUnitForZero=" << convertZeroToBaseUnit;
    os << " allowOriginalUnit=" << allowOriginalUnit;
    os << " allowNonmetricTimeUnits=" << allowNonmetricTimeUnits;
    os << " logarithmicUnitsPolicy=" << preferenceName(logarithmicUnitsPolicy);
    os << " bitBasedUnitsPolicy=" << preferenceName(bitBasedUnitsPolicy);
    os << " binaryPrefixPolicy=" << preferenceName(binaryPrefixPolicy);
    os << " kilobyteThreshold=" << kilobyteThreshold;
    return os.str();
}

bool UnitConversion::matches(const Unit *unit, const char *unitName)
{
    // short name
    if (strcmp(unit->name, unitName) == 0)
        return true;
    // if unit is at least 3 chars, look up by long name, case insensitive ("herz", "milliwatt")
    if (unitName[1] && unitName[2] && strcasecmp(unit->longName, unitName) == 0)
        return true;
    return false;
}

inline unsigned lc(char c) {return (unsigned)c | 0x20;} // assuming ASCII letters

inline unsigned UnitConversion::hashCode(const char *unitName)
{
    unsigned result = lc(unitName[0]);
    if (!unitName[1]) return result;
    result = result*65 + lc(unitName[1]);
    if (!unitName[2]) return result;
    result = result*65 + lc(unitName[2]);
    for (const char *s = unitName+3; *s; s++)
        result = result*65 + lc(*s);
    return result;
}

const UnitConversion::Unit *UnitConversion::lookupUnit(const char *unitName)
{
    if (!unitName || !*unitName)
        return nullptr; // nullptr or empty string is not a unit

    // hash table lookup
    unsigned hash = hashCode(unitName);
    for (unsigned pos = hash & (HASHTABLESIZE-1); hashTable[pos]; pos = (pos+1) & (HASHTABLESIZE-1))
        if (matches(hashTable[pos], unitName))
            return hashTable[pos];

    // not found, maybe it's a plural form of long name, retry by slicing off the trailing "s"
    int len = strlen(unitName);
    if (len >= 4 && unitName[len-1] == 's')
        return lookupUnit(std::string(unitName, len-1).c_str());

    return nullptr;
}

const UnitConversion::Unit *UnitConversion::getUnit(const char *unitName)
{
    const Unit *unit = lookupUnit(unitName);
    Assert(unit);
    return unit;
}

std::vector<const UnitConversion::Unit *> UnitConversion::lookupUnits(const std::vector<const char *>& unitNames)
{
    std::vector<const Unit *> result;
    for (const char *unitName : unitNames)
        result.push_back(getUnit(unitName));
    return result;
}

void UnitConversion::insert(const char *key, const Unit *unit)
{
    int localCollisions = 0;
    unsigned hash = hashCode(key);
    unsigned pos;
    for (pos = hash & (HASHTABLESIZE-1); hashTable[pos]; pos = (pos+1) & (HASHTABLESIZE-1))
        localCollisions++;
    hashTable[pos] = unit;
    numCollisions += localCollisions;
    Assert(localCollisions <= 2); // usually 0, rarely 1 collision observed at the time of writing
}

void UnitConversion::init()
{
    fillHashtable();
    fillUnitData();
}

void UnitConversion::fillHashtable()
{
    for (const Unit *p = unitTable; p->name; p++) {
        insert(p->name, p);
        insert(p->longName, p);
    }
    Assert(numCollisions <= 25); // 21 collisions observed at the time of writing
}

inline bool isWholeNumber(double x) { return fabs(x - floor(x+0.5)) < 1e-15; }

void UnitConversion::fillUnitData()
{
    for (Unit *unit = unitTable; unit->name; unit++) {
        unit->baseUnit = getUnit(unit->baseUnitName);
        unit->isByteBased = strstr(unit->longName, "byte") != nullptr;

        for (std::string unitName : opp_splitandtrim(opp_emptytodefault(unit->bestUnitCandidatesList, unit->name))) {  // at least itself is a candidate if candidates are not explicitly listed
            unit->bestUnitCandidateNames.push_back(unitName.c_str());
            unit->bestUnitCandidates.push_back(getUnit(unitName.c_str()));
        }

        const char *n = unit->name;
        if (unit->mapping == LOG10)
            unit->prefixType = DECIMAL;
        else if (opp_stringbeginswith(n, "Ki") || opp_stringbeginswith(n, "Mi") || opp_stringbeginswith(n, "Gi") || opp_stringbeginswith(n, "Ti") || opp_streq(n, "b") || opp_streq(n, "B"))
            unit->prefixType = BINARY;
        else if (isWholeNumber(log10(unit->isByteBased ? unit->mult/8 : unit->mult)))
            unit->prefixType = DECIMAL;
        else
            unit->prefixType = TRADITIONAL;
    }

    // filling in compatibleUnits depends on baseUnit pointers already being filled in
    for (const Unit *p = unitTable; p->name; p++) {
        Unit *unit = const_cast<Unit *>(p);
        for (const Unit *candidate = unitTable; candidate->name; candidate++) {
            if (areCompatibleUnits(candidate, unit)) {
                unit->compatibleUnitNames.push_back(candidate->name);
                unit->compatibleUnits.push_back(candidate);
            }
        }
    }

    bitsUnit = getUnit("b");
    bytesUnit = getUnit("B");
    secondsUnit = getUnit("s");
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

bool UnitConversion::readUnit(const char *& s, std::string& unitName)
{
    unitName = "";
    while (opp_isspace(*s))
        s++;
    while (opp_isalpha(*s))
        unitName.append(1, *s++);
    while (opp_isspace(*s))
        s++;
    return !unitName.empty();
}

double UnitConversion::parseQuantity(const char *str, const char *expectedUnitName)
{
    std::string unitName;
    double value = parseQuantity(str, unitName);
    return convertUnit(value, unitName.c_str(), expectedUnitName);
}

double UnitConversion::parseQuantity(const char *str, std::string& unitName)
{
    double result = 0;
    unitName = "";
    const char *s = str;

    // read first number and unit
    if (!readNumber(s, result))
        throw opp_runtime_error("Syntax error parsing quantity '%s': Must begin with a number", str);
    if (!readUnit(s, unitName)) {
        // special case: plain number without unitName
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
        if (!isLinearUnit(unitName.c_str()) || !isLinearUnit(tmpUnit.c_str()))
            throw opp_runtime_error("Error in quantity '%s': Concatenated quantity literals are not supported with non-linear units like dB", str);
        double factor = getConversionFactor(unitName.c_str(), tmpUnit.c_str());
        if (factor == 0)
            throw opp_runtime_error("Error in quantity '%s': Unit %s does not match %s",
                    str, getUnitDescription(tmpUnit.c_str()).c_str(), getUnitDescription(unitName.c_str()).c_str());
        if (factor <= 1)
            throw opp_runtime_error("Error in quantity '%s': Units must be in strictly decreasing order of magnitude", str); // to reduce chance of confusing a compound quantity with a list

        // do the conversion
        result = result * factor + d;
        unitName = tmpUnit;
    }

    if (minus)
        result = -result;

    // must be at the end of the input string
    if (*s)
        throw opp_runtime_error("Syntax error parsing quantity '%s'", str);

    // success
    return result;
}

std::string UnitConversion::formatQuantity(double value, const char *unitName)
{
    char buf[32];
    opp_dtoa(buf, "%g", value);
    if (!opp_isempty(unitName)) {
        if (!std::isfinite(value))
            strcat(buf, " ");
        strcat(buf, unitName);
    }
    return buf;
}

std::string UnitConversion::getUnitDescription(const char *unitName)
{
    const Unit *unit = lookupUnit(unitName);
    std::string result = std::string("'")+unitName+"'";
    if (unit)
        result += std::string(" (") + unit->longName + ")";
    return result;
}

std::string UnitConversion::getConversionDescription(const char *unitName)
{
    std::stringstream os;
    const Unit *unit = lookupUnit(unitName);
    switch (unit->mapping) {
        case LINEAR: os << unit->mult << unit->baseUnitName; break;
        case LOG10:  os << unit->mult << "*log10(" << unit->baseUnitName << ")"; break;
    }
    return os.str();
}

double UnitConversion::getConversionFactor(const char *unitName, const char *targetUnitName)
{
    // if there are no units or if units are the same, no conversion is needed
    if (unitName == targetUnitName || opp_strcmp(unitName, targetUnitName) == 0)
        return 1.0;

    // if only one unit is given, that's an error
    if (opp_isempty(unitName) || opp_isempty(targetUnitName))
        return 0;  // cannot convert

    // look up both units
    const Unit *unit = lookupUnit(unitName);
    const Unit *targetUnit = lookupUnit(targetUnitName);
    if (unit == nullptr || targetUnit == nullptr)
        return 0;  // one of them is custom unit, cannot convert

    return tryGetConversionFactor(unit, targetUnit);
}

double UnitConversion::tryGetConversionFactor(const Unit *unit, const Unit *targetUnit)
{
    // if they are the same units, or one is the base unit of the other, we're done
    if (unit == targetUnit)
        return 1.0;
    if (unit->baseUnit == targetUnit && unit->mapping == LINEAR)
        return unit->mult;
    if (unit == targetUnit->baseUnit && targetUnit->mapping == LINEAR)
        return 1.0 / targetUnit->mult;

    // convert unit to the base, and try again
    if (unit != unit->baseUnit && unit->mapping == LINEAR) {
        const Unit *baseUnit = lookupUnit(unit->baseUnitName);
        return unit->mult * tryGetConversionFactor(baseUnit, targetUnit);
    }

    // try converting via the target unit's base
    if (targetUnit != targetUnit->baseUnit && targetUnit->mapping == LINEAR) {
        const Unit *targetBaseUnit = targetUnit->baseUnit;
        return tryGetConversionFactor(unit, targetBaseUnit) / targetUnit->mult;
    }

    return 0; // no conversion found
}

double UnitConversion::convertToBase(double value, const Unit *unit)
{
    switch (unit->mapping) {
        case LINEAR: return unit->mult * value;
        case LOG10:  return pow(10.0, value / unit->mult);
        default: throw opp_runtime_error("UnitConversion: invalid unit type");
    }
}

double UnitConversion::convertFromBase(double value, const Unit *unit)
{
    switch (unit->mapping) {
        case LINEAR: return value / unit->mult;
        case LOG10:  return unit->mult * log10(value);
        default: throw opp_runtime_error("UnitConversion: invalid unit type");
    }
}

double UnitConversion::convertUnit(double value, const char *unitName, const char *targetUnitName)
{
    // if there are no units or if units are the same, no conversion is needed
    if (unitName == targetUnitName || opp_streq(unitName, targetUnitName))
        return value;

    // allow missing unit for NaN (NaN is NaN in every unit)
    if (std::isnan(value) && opp_isempty(unitName))
        return value;

    // if only one unit is given, that's an error
    if (opp_isempty(unitName) || opp_isempty(targetUnitName))
        cannotConvert(unitName, targetUnitName);

    // look up both units
    const Unit *unit = lookupUnit(unitName);
    const Unit *targetUnit = lookupUnit(targetUnitName);
    if (unit == nullptr || targetUnit == nullptr)
        cannotConvert(unitName, targetUnitName); // one of them is custom unit

    // convert
    double result = tryConvert(value, unit, targetUnit);
    if (std::isnan(result) && !std::isnan(value))
        cannotConvert(unitName, targetUnitName);
    return result;
}

void UnitConversion::cannotConvert(const char *unitName, const char *targetUnitName)
{
    throw opp_runtime_error("Cannot convert unit %s to %s",
            (opp_isempty(unitName) ? "none" : getUnitDescription(unitName).c_str()),
            (opp_isempty(targetUnitName) ? "none" : getUnitDescription(targetUnitName).c_str()));
}

double UnitConversion::tryConvert(double value, const Unit *unit, const Unit *targetUnit)
{
    // if they are the same units, or one is the base unit of the other, we're done
    if (unit == targetUnit)
        return value;
    if (unit->baseUnit == targetUnit)
        return convertToBase(value, unit);
    if (unit == targetUnit->baseUnit)
        return convertFromBase(value, targetUnit);

    // convert unit to the base, and try again
    if (unit != unit->baseUnit) {
        const Unit *baseUnit = unit->baseUnit;
        return tryConvert(convertToBase(value, unit), baseUnit, targetUnit);
    }

    // try converting via the target unitName's base
    if (targetUnit->name != targetUnit->baseUnitName) {
        const Unit *targetBaseUnit = targetUnit->baseUnit;
        return convertFromBase(tryConvert(value, unit, targetBaseUnit), targetUnit);
    }

    return NaN;
}

static std::vector<const char *> emptyList; // note: cannot be moved into the function for thread safety

const std::vector<const char *>& UnitConversion::getCompatibleKnownUnits(const char *unitName)
{
    const Unit *unit = lookupUnit(unitName);
    return unit ? unit->compatibleUnitNames : emptyList;
}

inline void bump(double& score, double amount, const char *note)
{
    score += amount;
    if (debugScoring)
        std::cout << "  " << (amount>0 ? "+" : "") << amount << ", reason: " << note << " -> score=" << score << endl;
}

double UnitConversion::calculateUnitScore(double originalValue, const Unit *originalUnit, const Unit *unit, const Options& options, bool considerPreferences)
{
    Assert(unit);
    Assert(originalUnit);

    if (originalValue < 0 && originalUnit->mapping == LINEAR) {
        if (unit->mapping == LOG10)
            return NaN; // negative quantities cannot be represent with logarithmic units
        else
            originalValue = std::fabs(originalValue);
    }

    double value = tryConvert(originalValue, originalUnit, unit);
    Assert(!std::isnan(value)); // conversion successful

    if (debugScoring)
        std::cout << "\n" << originalValue << originalUnit->name << " -> " << value << unit->name << " ?\n";

    double score = 0;

    // handle bit-byte-related units separately
    if (unit->baseUnit == bitsUnit) {
        switch (options.bitBasedUnitsPolicy) {
            case PREFER: if (!unit->isByteBased) bump(score, 1, "prefer bits"); break;
            case AVOID:  if (unit->isByteBased) bump(score, 1, "prefer bytes"); break;
            case KEEP:   if (unit->isByteBased == originalUnit->isByteBased) bump(score, 10, "keep bit/byte"); break;
        }
        if (unit == bitsUnit || unit == bytesUnit)
            bump(score, 1, "B/b qualifies both as DECIMAL and BINARY prefix");  // bit and byte are both DECIMAL and BINARY prefixes, so collect the bonus for matching prefixTypePreference
        else {
            switch (options.binaryPrefixPolicy) {
                case PREFER: if (unit->prefixType == BINARY) bump(score, 1, "prefer binary prefixes for bits/bytes"); break;
                case AVOID:  if (unit->prefixType != BINARY) bump(score, 1, "prefer decimal prefixes for bits/bytes"); break;
                case KEEP:   if (unit->prefixType == originalUnit->prefixType) bump(score, 1, "keep prefix type of bit/byte unit"); break;
            }
        }
        if ((unit == bytesUnit || unit == bitsUnit) && value >= 1000 && value < options.kilobyteThreshold)
            bump(score, 1, "below kilobyteThreshold");

        // prevent a whole number of bits from being converted into a fractional number of bytes, at least under a certain threshold
        if (unit == bitsUnit && originalUnit == bitsUnit && isWholeNumber(originalValue) && !isWholeNumber(originalValue/8) && originalValue < options.kilobyteThreshold)
            bump(score, 5, "to prevent conversion into fractional bytes");

        if (options.preferSmallWholeNumbersForBitByte && isWholeNumber(originalValue) && isWholeNumber(value) && value < 1000)  // the isWholeNumber(originalValue) is somewhat questionable, and the limit of 1000 is somewhat arbirary
            bump(score, 5, "for keeping the result a whole number for bit/byte quantity");

    }
    else if (considerPreferences) {
        // reward units that are listed among the best-unitName candidates
        if (contains(originalUnit->bestUnitCandidates, unit))
            bump(score, 20, "among bestUnitCandidates");

        if (unit->baseUnit == secondsUnit) {
            if (unit->prefixType == TRADITIONAL && !options.allowNonmetricTimeUnits)
                bump(score, -1, "penalize nonmetric time unit");
        }
        else if (unit->prefixType == TRADITIONAL) {
            bump(score, -1, "penalize TRADITIONAL prefix");
        }

        switch (options.logarithmicUnitsPolicy) {
            case PREFER: if (unit->mapping == LOG10) bump(score, 30, "prefer logarithmic units"); break;
            case AVOID:  if (unit->mapping != LOG10) bump(score, 30, "avoid logarithmic units"); break;
            case KEEP:   if (unit->mapping == originalUnit->mapping) bump(score, 30, "keep unit logarithmic/linear"); break;
        }

    }

    if (unit->mapping == LINEAR) {
        // compress value into range 0..1
        // note: double's exponent range is about -308..+308, overestimate it with 620
        double compressedValue01 = log10(value) / 620 + 0.5;

        bump(score, 1, "using linear unit");

        if (value >= 1) {
            bump(score, 10, "value >= 1");
            bump(score, -compressedValue01, "-1*compressedValue (for value >= 1)"); // smaller values preferred
        }
        else {
            bump(score, compressedValue01, "compressedValue01 (for value < 1)"); // greater (closer-to-1) values preferred
        }
    }
    else { // LOG10
        double compressedValue01 = atan(value/10) / 3.1416 + 0.5;

        if (value >= 0) {
            bump(score, 10, "value >= 0");
            bump(score, -compressedValue01, "-1*compressedValue (for value >= 0)"); // smaller values preferred
        }
        else {
            bump(score, compressedValue01, "compressedValue01 (for value < 0)"); // greater (closer-to-1) values preferred
        }
    }

    return score;
}

static const std::vector<const char *> emptyNameList;
static const UnitConversion::Options defaultOptions;

const char *UnitConversion::getBestUnit(double d, const char *unitName)
{
    return getBestUnit(d, unitName, emptyNameList, defaultOptions);
}

const char *UnitConversion::getBestUnit(double value, const char *unitName, const std::vector<const char *>& allowedUnitNames, const Options& options)
{
    const Unit *unit = lookupUnit(unitName);
    if (unit == nullptr)
        return unitName;

    // filter list to all compatible units
    std::vector<const Unit *> allowedCompatibleUnits;
    for (const char *name : allowedUnitNames) {
        const Unit *u = lookupUnit(name);
        if (u && (u->baseUnit == unit->baseUnit || contains(unit->compatibleUnits, u))) {
            bool isIllegal = u->mapping == LOG10 && unit->mapping == LINEAR && value < 0; // cannot represent negative quantities in logarithmic units
            if (!isIllegal)
                allowedCompatibleUnits.push_back(u);
        }
    }
    bool considerPreferences = allowedCompatibleUnits.empty();

    // empty allowedCompatibleUnits means "all compatible units"
    if (allowedCompatibleUnits.empty())
        allowedCompatibleUnits = unit->compatibleUnits;

    if (options.allowOriginalUnit && !contains(allowedCompatibleUnits, unit))
        allowedCompatibleUnits.push_back(unit);

    return getBestUnit(value, unit, allowedCompatibleUnits, options, considerPreferences);
}

const char *UnitConversion::getBestUnit(double value, const Unit *unit, const std::vector<const Unit *>& candidates, const Options& options, bool considerPreferences)
{
    Assert(unit != nullptr);

    // if value is zero, infinite and NaN are special cases; return either the original unit or its base unit
    if ((value == 0 && unit->mapping == LINEAR) || !std::isfinite(value)) {
        if (!options.convertZeroToBaseUnit)
            return unit->name;
        if (unit->baseUnit == bitsUnit) {
            switch (options.bitBasedUnitsPolicy) {
                case PREFER: return bitsUnit->name;
                case AVOID: return bytesUnit->name;
                case KEEP: return unit->isByteBased ? bytesUnit->name : bitsUnit->name;
            }
        }
        else {
            return unit->baseUnit->name;
        }
    }

    Assert(!candidates.empty());

    // score the candidates
    std::vector<double> scores;
    for (const Unit *u : candidates)
        scores.push_back(calculateUnitScore(value, unit, u, options, considerPreferences));

    // pick best one
    auto it = std::max_element(scores.begin(), scores.end());
    int index = it - scores.begin();
    const Unit *bestUnit = candidates[index];
    return bestUnit->name;
}

const char *UnitConversion::getShortName(const char *unitName)
{
    const Unit *unit = lookupUnit(unitName);
    return unit ? unit->name : unitName;
}

const char *UnitConversion::getLongName(const char *unitName)
{
    const Unit *unit = lookupUnit(unitName);
    return unit ? unit->longName : unitName;
}

std::string UnitConversion::getLongName(const char *unitName, bool plural)
{
    const Unit *unit = lookupUnit(unitName);
    const char *s = unit ? unit->longName : unitName;
    if (!plural || s[strlen(s)-1] == 's')
        return s;
    else {
        // meter -> meters, meter/sec -> meters/sec (not meter/secs!)
        const char *slash = strchr(s, '/');
        if (slash == nullptr || slash == s)
            return std::string(s) + "s";
        else if (slash[-1] == 's')
            return s;
        else
            return std::string(s,slash-s) + "s" + std::string(slash);
    }
}

const char *UnitConversion::getBaseUnit(const char *unitName)
{
    const Unit *unit = lookupUnit(unitName);
    return unit ? unit->baseUnitName : nullptr;
}

bool UnitConversion::isLinearUnit(const char *unitName)
{
    const Unit *unit = lookupUnit(unitName);
    return unit ? unit->mapping == LINEAR : true;
}

bool UnitConversion::areCompatibleUnits(const char *unit1Name, const char *unit2Name)
{
    if (opp_streq(unit1Name, unit2Name))
        return true;

    const Unit *unit1 = lookupUnit(unit1Name);
    const Unit *unit2 = lookupUnit(unit2Name);
    if (unit1 == nullptr || unit2 == nullptr)
        return false;

    return areCompatibleUnits(unit1, unit2);
}

bool UnitConversion::areCompatibleUnits(const Unit *unit1, const Unit *unit2)
{
    if (unit1->baseUnit == unit2->baseUnit)
        return true;
    if (unit1->baseUnit != unit1)
        return areCompatibleUnits(unit1->baseUnit, unit2);
    if (unit2->baseUnit != unit2)
        return areCompatibleUnits(unit1, unit2->baseUnit);
    return false;
}

std::vector<const char *> UnitConversion::getKnownUnits()
{
    std::vector<const char *> result;
    for (int i = 0; unitTable[i].name; i++)
        result.push_back(unitTable[i].name);
    return result;
}

}  // namespace common
}  // namespace omnetpp

