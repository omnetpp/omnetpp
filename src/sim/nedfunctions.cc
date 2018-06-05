//=========================================================================
//  NEDFUNCTIONS.CC - part of
//
//                    OMNeT++/OMNEST
//             Discrete System Simulation in C++
//
//=========================================================================

/*--------------------------------------------------------------*
  Copyright (C) 1992-2017 Andras Varga
  Copyright (C) 2006-2017 OpenSim Ltd.

  This file is distributed WITHOUT ANY WARRANTY. See the file
  `license' for details on this and other legal matters.
*--------------------------------------------------------------*/

#include <cmath>
#include <cstring>
#include <algorithm>
#include "common/stringutil.h"
#include "common/opp_ctype.h"
#include "common/unitconversion.h"
#include "omnetpp/distrib.h"
#include "omnetpp/cnedmathfunction.h"
#include "omnetpp/cnedfunction.h"
#include "omnetpp/cexception.h"
#include "omnetpp/cstringtokenizer.h"
#include "omnetpp/cconfiguration.h"
#include "omnetpp/ccomponenttype.h"
#include "omnetpp/csimulation.h"
#include "omnetpp/cmodule.h"

using namespace omnetpp::common;

namespace omnetpp {

//FIXME cDynamicExpression to add function name to exceptions thrown from functions

void nedfunctions_dummy() {} //see util.cc

#define DEF(FUNCTION, SIGNATURE, CATEGORY, DESCRIPTION) \
        cNedValue FUNCTION(cComponent *context, cNedValue argv[], int argc); \
        Define_NED_Function2(FUNCTION, SIGNATURE, CATEGORY, DESCRIPTION);

//
// NED math.h functions
//

Define_NED_Math_Function3(acos, 1,  "math", "Trigonometric function; see standard C function of the same name")
Define_NED_Math_Function3(asin, 1,  "math", "Trigonometric function; see standard C function of the same name")
Define_NED_Math_Function3(atan, 1,  "math", "Trigonometric function; see standard C function of the same name")
Define_NED_Math_Function3(atan2, 2, "math", "Trigonometric function; see standard C function of the same name")

Define_NED_Math_Function3(sin, 1,   "math", "Trigonometric function; see standard C function of the same name")
Define_NED_Math_Function3(cos, 1,   "math", "Trigonometric function; see standard C function of the same name")
Define_NED_Math_Function3(tan, 1,   "math", "Trigonometric function; see standard C function of the same name")

Define_NED_Math_Function3(ceil, 1,  "math", "Rounds down; see standard C function of the same name")
Define_NED_Math_Function3(floor, 1, "math", "Rounds up; see standard C function of the same name")

Define_NED_Math_Function3(exp, 1,   "math", "Exponential; see standard C function of the same name")
Define_NED_Math_Function3(pow, 2,   "math", "Power; see standard C function of the same name")
Define_NED_Math_Function3(sqrt, 1,  "math", "Square root; see standard C function of the same name")

Define_NED_Math_Function3(hypot, 2, "math", "Length of the hypotenuse; see standard C function of the same name")

Define_NED_Math_Function3(log, 1,   "math", "Natural logarithm; see standard C function of the same name")
Define_NED_Math_Function3(log10, 1, "math", "Base-10 logarithm; see standard C function of the same name")


static cNedValue toUnit(const char *targetUnit, cComponent *contextComponent, cNedValue argv[], int argc)
{
    if (argv[0].getUnit() == nullptr)
        argv[0].setUnit(targetUnit);
    else {
        argv[0].convertToDouble();
        argv[0].convertTo(targetUnit);
    }
    return argv[0];
}

#define DEFU(UNIT, LONGNAME) \
        cNedValue nedf_ ## UNIT(cComponent *context, cNedValue argv[], int argc) { return toUnit(#UNIT, context, argv, argc); } \
        Define_NED_Function2(nedf_ ## UNIT, \
            "quantity " #UNIT "(quantity x)", \
            "units/conversion", \
            "Converts to " #UNIT " (" LONGNAME ") from a compatible unit or a dimensionless number.");

// note: currently unit conversion functions must be registered one-by-one because
// there is no way to pass the target unit into a generic function like toUnit().
DEFU(s, "second")
DEFU(d, "day")
DEFU(h, "hour")
DEFU(ms, "millisecond")
DEFU(us, "microsecond")
DEFU(ns, "nanosecond")
DEFU(ps, "picosecond")
DEFU(fs, "femtosecond")
DEFU(as, "attosecond")
DEFU(bps, "bit/sec")
DEFU(kbps, "kilobit/sec")
DEFU(Mbps, "megabit/sec")
DEFU(Gbps, "gigabit/sec")
DEFU(Tbps, "terabit/sec")
DEFU(B, "byte")
DEFU(KiB, "kibibyte")
DEFU(MiB, "mebibyte")
DEFU(GiB, "gibibyte")
DEFU(TiB, "tebibyte")
DEFU(kB, "kilobyte")
DEFU(MB, "megabyte")
DEFU(GB, "gigabyte")
DEFU(TB, "terabyte")
DEFU(b, "bit")
DEFU(Kib, "kibibit")
DEFU(Mib, "mebibit")
DEFU(Gib, "gibibit")
DEFU(Tib, "tebibit")
DEFU(kb, "kilobit")
DEFU(Mb, "megabit")
DEFU(Gb, "gigabit")
DEFU(Tb, "terabit")
DEFU(rad, "radian")
DEFU(deg, "degree")
DEFU(m, "meter")
DEFU(mm, "millimeter")
DEFU(cm, "centimeter")
DEFU(km, "kilometer")
DEFU(W, "watt")
DEFU(mW, "milliwatt")
DEFU(uW, "microwatt")
DEFU(nW, "nanowatt")
DEFU(pW, "picowatt")
DEFU(fW, "femtowatt")
DEFU(kW, "kilowatt")
DEFU(MW, "megawatt")
DEFU(GW, "gigawatt")
DEFU(Hz, "hertz")
DEFU(kHz, "kilohertz")
DEFU(MHz, "megahertz")
DEFU(GHz, "gigahertz")
DEFU(THz, "terahertz")
DEFU(kg, "kilogram")
DEFU(g, "gram")
DEFU(K, "kelvin")
DEFU(J, "joule")
DEFU(kJ, "kilojoule")
DEFU(MJ, "megajoule")
DEFU(V, "volt")
DEFU(kV, "kilovolt")
DEFU(mV, "millivolt")
DEFU(A, "ampere")
DEFU(mA, "milliampere")
DEFU(uA, "microampere")
DEFU(mps, "meter/sec")
DEFU(kmps, "kilometer/sec")
DEFU(kmph, "kilometer/hour")
DEFU(Ohm, "ohm")
DEFU(mOhm, "milliohm")
DEFU(kOhm, "kiloohm")
DEFU(MOhm, "megaohm")
DEFU(C, "coulomb")
DEFU(As, "ampere-second")
DEFU(mAs, "milliampere-second")
DEFU(Ah, "ampere-hour")
DEFU(mAh, "milliampere-hour")
DEFU(ratio, "ratio")
DEFU(pct, "percent")
DEFU(dBW, "decibel-watt")
DEFU(dBm, "decibel-milliwatt")
DEFU(dBmW, "decibel-milliwatt")
DEFU(dBV, "decibel-volt")
DEFU(dBmV, "decibel-millivolt")
DEFU(dBA, "decibel-ampere")
DEFU(dBmA, "decibel-milliampere")
DEFU(dB, "decibel")


DEF(nedf_fabs,
    "quantity fabs(quantity x)",
    "math",
    "Returns the absolute value of the quantity.")

cNedValue nedf_fabs(cComponent *contextComponent, cNedValue argv[], int argc)
{
    return cNedValue(fabs((double)argv[0]), argv[0].getUnit());
}

DEF(nedf_fmod,
    "quantity fmod(quantity x, quantity y)",
    "math",
    "Returns the floating-point remainder of x/y; unit conversion takes place if needed.")

cNedValue nedf_fmod(cComponent *contextComponent, cNedValue argv[], int argc)
{
    double argv1converted = argv[1].doubleValueInUnit(argv[0].getUnit());
    return cNedValue(fmod((double)argv[0], argv1converted), argv[0].getUnit());
}

DEF(nedf_min,
    "quantity min(quantity a, quantity b)",
    "math",
    "Returns the smaller one of the two quantities; unit conversion takes place if needed.")
cNedValue nedf_min(cComponent *contextComponent, cNedValue argv[], int argc)
{
    double argv1converted = argv[1].doubleValueInUnit(argv[0].getUnit());
    return (double)argv[0] < argv1converted ? argv[0] : argv[1];
}

DEF(nedf_max,
    "quantity max(quantity a, quantity b)",
    "math",
    "Returns the greater one of the two quantities; unit conversion takes place if needed.")

cNedValue nedf_max(cComponent *contextComponent, cNedValue argv[], int argc)
{
    double argv1converted = argv[1].doubleValueInUnit(argv[0].getUnit());
    return (double)argv[0] < argv1converted ? argv[1] : argv[0];
}


//
// Unit handling
//

DEF(nedf_dropUnit,
    "double dropUnit(quantity x)",
    "units",
    "Removes the unit of measurement from quantity x.")

cNedValue nedf_dropUnit(cComponent *contextComponent, cNedValue argv[], int argc)
{
    argv[0].setUnit(nullptr);
    return argv[0];
}

DEF(nedf_replaceUnit,
    "quantity replaceUnit(quantity x, string unit)",
    "units",
    "Replaces the unit of x with the given unit.")

cNedValue nedf_replaceUnit(cComponent *contextComponent, cNedValue argv[], int argc)
{
    const char *newUnit = cNedValue::getPooled((const char *)argv[1]);
    argv[0].setUnit(newUnit);
    return argv[0];
}

DEF(nedf_convertUnit,
    "quantity convertUnit(quantity x, string unit)",
    "units",
    "Converts x to the given unit.")

cNedValue nedf_convertUnit(cComponent *contextComponent, cNedValue argv[], int argc)
{
    const char *newUnit = cNedValue::getPooled((const char *)argv[1]);
    argv[0].convertTo(newUnit);
    return argv[0];
}

DEF(nedf_unitOf,
    "string unitOf(quantity x)",
    "units",
    "Returns the unit of the given quantity.")

cNedValue nedf_unitOf(cComponent *contextComponent, cNedValue argv[], int argc)
{
    return argv[0].getUnit();
}


//
// String manipulation functions.
//

DEF(nedf_length,
    "int length(string s)",
    "strings",
    "Returns the length of the string")

cNedValue nedf_length(cComponent *contextComponent, cNedValue argv[], int argc)
{
    return (intpar_t)argv[0].stdstringValue().size();
}

DEF(nedf_contains,
    "bool contains(string s, string substr)",
    "strings",
    "Returns true if string s contains substr as substring")

cNedValue nedf_contains(cComponent *contextComponent, cNedValue argv[], int argc)
{
    return argv[0].stdstringValue().find(argv[1].stdstringValue()) != std::string::npos;
}

DEF(nedf_substring,
    "string substring(string s, int pos, int len?)",
    "strings",
    "Return the substring of s starting at the given position, either to the end of the string or maximum len characters")

cNedValue nedf_substring(cComponent *contextComponent, cNedValue argv[], int argc)
{
    int size = argv[0].stdstringValue().size();
    int index = (int)argv[1];
    int length = argc == 3 ? (int)argv[2] : size - index;

    if (index < 0 || index > size)
        throw cRuntimeError("substring(): Index out of bounds");
    if (length < 0)
        throw cRuntimeError("substring(): Length is negative");
    return argv[0].stdstringValue().substr(index, length);
}

DEF(nedf_substringBefore,
    "string substringBefore(string s, string substr)",
    "strings",
    "Returns the substring of s before the first occurrence of substr, or the empty string if s does not contain substr.")

cNedValue nedf_substringBefore(cComponent *contextComponent, cNedValue argv[], int argc)
{
    size_t pos = argv[0].stdstringValue().find(argv[1].stdstringValue());
    return pos == std::string::npos ? "" : argv[0].stdstringValue().substr(0, pos);
}

DEF(nedf_substringAfter,
    "string substringAfter(string s, string substr)",
    "strings",
    "Returns the substring of s after the first occurrence of substr, or the empty string if s does not contain substr.")

cNedValue nedf_substringAfter(cComponent *contextComponent, cNedValue argv[], int argc)
{
    size_t pos = argv[0].stdstringValue().find(argv[1].stdstringValue());
    return pos == std::string::npos ? "" : argv[0].stdstringValue().substr(pos + argv[1].stdstringValue().size());
}

DEF(nedf_substringBeforeLast,
    "string substringBeforeLast(string s, string substr)",
    "strings",
    "Returns the substring of s before the last occurrence of substr, or the empty string if s does not contain substr.")

cNedValue nedf_substringBeforeLast(cComponent *contextComponent, cNedValue argv[], int argc)
{
    size_t pos = argv[0].stdstringValue().rfind(argv[1].stdstringValue());
    return pos == std::string::npos ? "" : argv[0].stdstringValue().substr(0, pos);
}

DEF(nedf_substringAfterLast,
    "string substringAfterLast(string s, string substr)",
    "strings",
    "Returns the substring of s after the last occurrence of substr, or the empty string if s does not contain substr.")

cNedValue nedf_substringAfterLast(cComponent *contextComponent, cNedValue argv[], int argc)
{
    size_t pos = argv[0].stdstringValue().rfind(argv[1].stdstringValue());
    return pos == std::string::npos ? "" : argv[0].stdstringValue().substr(pos + argv[1].stdstringValue().size());
}

DEF(nedf_startsWith,
    "bool startsWith(string s, string substr)",
    "strings",
    "Returns true if s begins with the substring substr.")

cNedValue nedf_startsWith(cComponent *contextComponent, cNedValue argv[], int argc)
{
    return argv[0].stdstringValue().find(argv[1].stdstringValue()) == 0;
}

DEF(nedf_endsWith,
    "bool endsWith(string s, string substr)",
    "strings",
    "Returns true if s ends with the substring substr.")

cNedValue nedf_endsWith(cComponent *contextComponent, cNedValue argv[], int argc)
{
    return argv[0].stdstringValue().rfind(argv[1].stdstringValue()) == argv[0].stdstringValue().size() - argv[1].stdstringValue().size();
}

DEF(nedf_tail,
    "string tail(string s, int len)",
    "strings",
    "Returns the last len character of s, or the full s if it is shorter than len characters.")

cNedValue nedf_tail(cComponent *contextComponent, cNedValue argv[], int argc)
{
    int length = (int)argv[1];
    if (length < 0)
        throw cRuntimeError("tail(): Length is negative");
    int size = (int)argv[0].stdstringValue().size();
    return argv[0].stdstringValue().substr(std::max(0, size - length), size);
}

DEF(nedf_replace,
    "string replace(string s, string substr, string repl, int startPos?)",
    "strings",
    "Replaces all occurrences of substr in s with the string repl. If startPos is given, search begins from position startPos in s.")

cNedValue nedf_replace(cComponent *contextComponent, cNedValue argv[], int argc)
{
    std::string str = argv[0].stdstringValue();
    const std::string& search = argv[1].stdstringValue();
    const std::string& replacement = argv[2].stdstringValue();
    int index = 0;
    if (argc == 4) {
        index = (int)argv[3];
        if (index < 0)
            throw cRuntimeError("replace(): Start index is negative");
        if (index > (int)str.size())
            throw cRuntimeError("replace(): Start index out of bounds");
    }

    size_t searchSize = search.size();
    size_t replacementSize = replacement.size();
    while ((size_t)(index = str.find(search, index)) != std::string::npos) {
        str.replace(index, searchSize, replacement);
        index += replacementSize - searchSize + 1;
    }
    return str;
}

DEF(nedf_replaceFirst,
    "string replaceFirst(string s, string substr, string repl, int startPos?)",
    "strings",
    "Replaces the first occurrence of substr in s with the string repl. If startPos is given, search begins from position startPos in s.")

cNedValue nedf_replaceFirst(cComponent *contextComponent, cNedValue argv[], int argc)
{
    std::string str = argv[0].stdstringValue();
    const std::string& search = argv[1].stdstringValue();
    const std::string& replacement = argv[2].stdstringValue();
    int index = 0;
    if (argc == 4) {
        index = (int)argv[3];
        if (index < 0)
            throw cRuntimeError("replaceFirst(): Start index is negative");
        if (index > (int)str.size())
            throw cRuntimeError("replaceFirst(): Start index out of bounds");
    }

    size_t searchSize = search.size();
    if ((size_t)(index = str.find(search, index)) != std::string::npos)
        str.replace(index, searchSize, replacement);
    return str;
}

DEF(nedf_trim,
    "string trim(string s)",
    "strings",
    "Discards whitespace from the start and end of s, and returns the result.")

cNedValue nedf_trim(cComponent *contextComponent, cNedValue argv[], int argc)
{
    return opp_trim(argv[0].stdstringValue());
}

DEF(nedf_indexOf,
    "int indexOf(string s, string substr)",
    "strings",
    "Returns the position of the first occurrence of substring substr in s, or -1 if s does not contain substr.")

cNedValue nedf_indexOf(cComponent *contextComponent, cNedValue argv[], int argc)
{
    return (intpar_t)argv[0].stdstringValue().find(argv[1].stdstringValue());
}

DEF(nedf_choose,
    "string choose(int index, string list)",
    "strings",
    "Interprets list as a space-separated list, and returns the item at the given index. Negative and out-of-bounds indices cause an error.")

cNedValue nedf_choose(cComponent *contextComponent, cNedValue argv[], int argc)
{
    int index = (int)argv[0];
    if (index < 0)
        throw cRuntimeError("choose(): Negative index");
    cStringTokenizer tokenizer(argv[1].stdstringValue().c_str());
    for (int i = 0; i < index && tokenizer.hasMoreTokens(); i++)
        tokenizer.nextToken();
    if (!tokenizer.hasMoreTokens())
        throw cRuntimeError("choose(): Index out of bounds: %d", index);
    return tokenizer.nextToken();
}

DEF(nedf_toUpper,
    "string toUpper(string s)",
    "strings",
    "Converts s to all uppercase, and returns the result.")

cNedValue nedf_toUpper(cComponent *contextComponent, cNedValue argv[], int argc)
{
    std::string tmp = argv[0].stdstringValue();
    int length = tmp.length();
    for (int i = 0; i < length; i++)
        tmp[i] = opp_toupper(tmp[i]);
    return tmp;
}

DEF(nedf_toLower,
    "string toLower(string s)",
    "strings",
    "Converts s to all lowercase, and returns the result.")

cNedValue nedf_toLower(cComponent *contextComponent, cNedValue argv[], int argc)
{
    std::string tmp = argv[0].stdstringValue();
    int length = tmp.length();
    for (int i = 0; i < length; i++)
        tmp[i] = opp_tolower(tmp[i]);
    return tmp;
}

DEF(nedf_expand,
    "string expand(string s)",
    "strings",
    "Expands ${} variables (${configname}, ${runnumber}, etc.) in the given string, and returns the result.")

cNedValue nedf_expand(cComponent *contextComponent, cNedValue argv[], int argc)
{
    std::string tmp = argv[0].stdstringValue();
    tmp = getEnvir()->getConfig()->substituteVariables(tmp.c_str());
    return tmp;
}

DEF(nedf_int,
    "intquantity int(any x)",
    "conversion",
    "Converts x to int, and returns the result. A boolean argument becomes 0 or 1; a double is converted using floor(); a string is interpreted as number; an XML argument causes an error. Units are preserved.")

cNedValue nedf_int(cComponent *contextComponent, cNedValue argv[], int argc)
{
    switch (argv[0].getType()) {
        case cNedValue::BOOL:
            return (intpar_t)( (bool)argv[0] ? 1 : 0 );
        case cNedValue::INT:
            return argv[0];
        case cNedValue::DOUBLE:
            return cNedValue(checked_int_cast<intpar_t>(floor(argv[0].doubleValue())), argv[0].getUnit());
        case cNedValue::STRING: {
            std::string unit;
            double d = UnitConversion::parseQuantity(argv[0].stringValue(), unit);
            return cNedValue(checked_int_cast<intpar_t>(floor(d)), cNedValue::getPooled(unit.c_str()));
        }
        case cNedValue::XML:
            throw cRuntimeError("int(): Cannot convert xml to int");
        default:
            throw cRuntimeError("Internal error: Invalid cNedValue type");
    }
}

DEF(nedf_double,
    "quantity double(any x)",
    "conversion",
    "Converts x to double, and returns the result. A boolean argument becomes 0 or 1; a string is interpreted as number; an XML argument causes an error. Units are preserved.")

cNedValue nedf_double(cComponent *contextComponent, cNedValue argv[], int argc)
{
    switch (argv[0].getType()) {
        case cNedValue::BOOL:
            return (bool)argv[0] ? 1.0 : 0.0;
        case cNedValue::INT:
            return cNedValue((double)argv[0].intValue(), argv[0].getUnit());
        case cNedValue::DOUBLE:
            return argv[0];
        case cNedValue::STRING: {
            std::string unit;
            double d = UnitConversion::parseQuantity(argv[0].stringValue(), unit);
            return cNedValue(d, cNedValue::getPooled(unit.c_str()));
        }
        case cNedValue::XML:
            throw cRuntimeError("double(): Cannot convert xml to double");
        default:
            throw cRuntimeError("Internal error: Bad cNedValue type");
    }
}

DEF(nedf_string,
    "string string(any x)",
    "conversion",
    "Converts x to string, and returns the result.")

cNedValue nedf_string(cComponent *contextComponent, cNedValue argv[], int argc)
{
    return argv[0].str();
}


//
// Reflection
//

DEF(nedf_fullPath,
    "string fullPath()",
    "ned",
    "Returns the full path of the module or channel in context.")

cNedValue nedf_fullPath(cComponent *contextComponent, cNedValue argv[], int argc)
{
    return contextComponent->getFullPath();
}

DEF(nedf_fullName,
    "string fullName()",
    "ned",
    "Returns the full name of the module or channel in context.")

cNedValue nedf_fullName(cComponent *contextComponent, cNedValue argv[], int argc)
{
    return contextComponent->getFullName();
}

DEF(nedf_parentIndex,
    "int parentIndex()",
    "ned",
    "Returns the index of the parent module, which has to be part of module vector.")

cNedValue nedf_parentIndex(cComponent *contextComponent, cNedValue argv[], int argc)
{
    cModule *mod = contextComponent->getParentModule();
    if (!mod)
        throw cRuntimeError("parentIndex(): '%s' has no parent module", contextComponent->getFullPath().c_str());
    if (!mod->isVector())
        throw cRuntimeError("parentIndex(): Module '%s' is not a vector", mod->getFullPath().c_str());
    return (intpar_t)mod->getIndex();
}

DEF(nedf_ancestorIndex,
    "int ancestorIndex(int numLevels)",
    "ned",
    "Returns the index of the ancestor module numLevels levels above the module or channel in context.")

cNedValue nedf_ancestorIndex(cComponent *contextComponent, cNedValue argv[], int argc)
{
    int levels = (int)argv[0];
    if (levels < 0)
        throw cRuntimeError("ancestorIndex(): Negative number of levels");
    if (levels == 0 && !contextComponent->isModule())
        throw cRuntimeError("ancestorIndex(): Numlevels==0 and this is not a module");
    cModule *mod = dynamic_cast<cModule *>(contextComponent);
    for (int i = 0; mod && i < levels; i++)
        mod = mod->getParentModule();
    if (!mod)
        throw cRuntimeError("ancestorIndex(): Argument is larger than current nesting level");
    if (!mod->isVector())
        throw cRuntimeError("ancestorIndex(): Module '%s' is not a vector", mod->getFullPath().c_str());
    return (intpar_t)mod->getIndex();
}


//
// Random variate generation.
//

// continuous
DEF(nedf_uniform,
    "quantity uniform(quantity a, quantity b, int rng?)",
    "random/continuous",
    "Returns a random number from the Uniform distribution")

cNedValue nedf_uniform(cComponent *contextComponent, cNedValue argv[], int argc)
{
    int rng = argc == 3 ? (int)argv[2] : 0;
    double argv1converted = argv[1].doubleValueInUnit(argv[0].getUnit());
    return cNedValue(contextComponent->uniform((double)argv[0], argv1converted, rng), argv[0].getUnit());
}

DEF(nedf_exponential,
    "quantity exponential(quantity mean, int rng?)",
    "random/continuous",
    "Returns a random number from the Exponential distribution")

cNedValue nedf_exponential(cComponent *contextComponent, cNedValue argv[], int argc)
{
    int rng = argc == 2 ? (int)argv[1] : 0;
    return cNedValue(contextComponent->exponential((double)argv[0], rng), argv[0].getUnit());
}

DEF(nedf_normal,
    "quantity normal(quantity mean, quantity stddev, int rng?)",
    "random/continuous",
    "Returns a random number from the Normal distribution")

cNedValue nedf_normal(cComponent *contextComponent, cNedValue argv[], int argc)
{
    int rng = argc == 3 ? (int)argv[2] : 0;
    double argv1converted = argv[1].doubleValueInUnit(argv[0].getUnit());
    return cNedValue(contextComponent->normal((double)argv[0], argv1converted, rng), argv[0].getUnit());
}

DEF(nedf_truncnormal,
    "quantity truncnormal(quantity mean, quantity stddev, int rng?)",
    "random/continuous",
    "Returns a random number from the truncated Normal distribution")

cNedValue nedf_truncnormal(cComponent *contextComponent, cNedValue argv[], int argc)
{
    int rng = argc == 3 ? (int)argv[2] : 0;
    double argv1converted = argv[1].doubleValueInUnit(argv[0].getUnit());
    return cNedValue(contextComponent->truncnormal((double)argv[0], argv1converted, rng), argv[0].getUnit());
}

DEF(nedf_gamma_d,
    "quantity gamma_d(double alpha, quantity theta, int rng?)",
    "random/continuous",
    "Returns a random number from the Gamma distribution")

cNedValue nedf_gamma_d(cComponent *contextComponent, cNedValue argv[], int argc)
{
    int rng = argc == 3 ? (int)argv[2] : 0;
    return cNedValue(contextComponent->gamma_d((double)argv[0], (double)argv[1], rng), argv[1].getUnit());
}

DEF(nedf_beta,
    "double beta(double alpha1, double alpha2, int rng?)",
    "random/continuous",
    "Returns a random number from the Beta distribution")

cNedValue nedf_beta(cComponent *contextComponent, cNedValue argv[], int argc)
{
    int rng = argc == 3 ? (int)argv[2] : 0;
    argv[0] = contextComponent->beta((double)argv[0], (double)argv[1], rng);
    return argv[0];
}

DEF(nedf_erlang_k,
    "quantity erlang_k(int k, quantity mean, int rng?)",
    "random/continuous",
    "Returns a random number from the Erlang distribution")

cNedValue nedf_erlang_k(cComponent *contextComponent, cNedValue argv[], int argc)
{
    int k = (int)argv[0];
    if (k < 0)
        throw cRuntimeError("erlang_k(): K parameter (number of phases) must be positive (k=%d)", k);
    int rng = argc == 3 ? (int)argv[2] : 0;
    return cNedValue(contextComponent->erlang_k(k, (double)argv[1], rng), argv[1].getUnit());
}

DEF(nedf_chi_square,
    "double chi_square(int k, int rng?)",
    "random/continuous",
    "Returns a random number from the Chi-square distribution")

cNedValue nedf_chi_square(cComponent *contextComponent, cNedValue argv[], int argc)
{
    int k = (int)argv[0];
    if (k < 0)
        throw cRuntimeError("chi_square(): K parameter (degrees of freedom) must be positive (k=%d)", k);
    int rng = argc == 2 ? (int)argv[1] : 0;
    return contextComponent->chi_square(k, rng);
}

DEF(nedf_student_t,
    "double student_t(int i, int rng?)",
    "random/continuous",
    "Returns a random number from the Student-t distribution")

cNedValue nedf_student_t(cComponent *contextComponent, cNedValue argv[], int argc)
{
    int i = (int)argv[0];
    if (i < 0)
        throw cRuntimeError("student_t(): I parameter (degrees of freedom) must be positive (i=%d)", i);
    int rng = argc == 2 ? (int)argv[1] : 0;
    return contextComponent->student_t(i, rng);
}

DEF(nedf_cauchy,
    "quantity cauchy(quantity a, quantity b, int rng?)",
    "random/continuous",
    "Returns a random number from the Cauchy distribution")

cNedValue nedf_cauchy(cComponent *contextComponent, cNedValue argv[], int argc)
{
    int rng = argc == 3 ? (int)argv[2] : 0;
    double argv1converted = argv[1].doubleValueInUnit(argv[0].getUnit());
    return cNedValue(contextComponent->cauchy((double)argv[0], argv1converted, rng), argv[0].getUnit());
}

DEF(nedf_triang,
    "quantity triang(quantity a, quantity b, quantity c, int rng?)",
    "random/continuous",
    "Returns a random number from the Triangular distribution")

cNedValue nedf_triang(cComponent *contextComponent, cNedValue argv[], int argc)
{
    int rng = argc == 4 ? (int)argv[3] : 0;
    double argv1converted = argv[1].doubleValueInUnit(argv[0].getUnit());
    double argv2converted = argv[2].doubleValueInUnit(argv[0].getUnit());
    return cNedValue(contextComponent->triang((double)argv[0], argv1converted, argv2converted, rng), argv[0].getUnit());
}

DEF(nedf_lognormal,
    "double lognormal(double m, double w, int rng?)",
    "random/continuous",
    "Returns a random number from the Lognormal distribution")

cNedValue nedf_lognormal(cComponent *contextComponent, cNedValue argv[], int argc)
{
    int rng = argc == 3 ? (int)argv[2] : 0;
    return contextComponent->lognormal((double)argv[0], (double)argv[1], rng);
}

DEF(nedf_weibull,
    "quantity weibull(quantity a, quantity b, int rng?)",
    "random/continuous",
    "Returns a random number from the Weibull distribution")

cNedValue nedf_weibull(cComponent *contextComponent, cNedValue argv[], int argc)
{
    int rng = argc == 3 ? (int)argv[2] : 0;
    double argv1converted = argv[1].doubleValueInUnit(argv[0].getUnit());
    return cNedValue(contextComponent->weibull((double)argv[0], argv1converted, rng), argv[0].getUnit());
}

DEF(nedf_pareto_shifted,
    "quantity pareto_shifted(double a, quantity b, quantity c, int rng?)",
    "random/continuous",
    "Returns a random number from the Pareto-shifted distribution")

cNedValue nedf_pareto_shifted(cComponent *contextComponent, cNedValue argv[], int argc)
{
    int rng = argc == 4 ? (int)argv[3] : 0;
    double argv2converted = argv[2].doubleValueInUnit(argv[1].getUnit());
    return cNedValue(contextComponent->pareto_shifted((double)argv[0], (double)argv[1], argv2converted, rng), argv[1].getUnit());
}

// discrete

DEF(nedf_intuniform,
    "int intuniform(intquantity a, intquantity b, int rng?)",
    "random/discrete",
    "Returns a random integer uniformly distributed over [a, b]")

cNedValue nedf_intuniform(cComponent *contextComponent, cNedValue argv[], int argc)
{
    int rng = argc == 3 ? (int)argv[2] : 0;
    if (opp_strcmp(argv[0].getUnit(), argv[1].getUnit()) != 0)
        throw cRuntimeError("intuniform(%s,%s): arguments must have the same unit", argv[0].stdstringValue().c_str(), argv[1].stdstringValue().c_str()); //TODO convert to smaller unit instead
    return cNedValue((intpar_t)contextComponent->intuniform((int)argv[0], (int)argv[1], rng), argv[1].getUnit());
}

DEF(nedf_bernoulli,
    "int bernoulli(double p, int rng?)",
    "random/discrete",
    "Returns a random number from the Bernoulli distribution")

cNedValue nedf_bernoulli(cComponent *contextComponent, cNedValue argv[], int argc)
{
    int rng = argc == 2 ? (int)argv[1] : 0;
    return (intpar_t)contextComponent->bernoulli((double)argv[0], rng);
}

DEF(nedf_binomial,
    "int binomial(int n, double p, int rng?)",
    "random/discrete",
    "Returns a random number from the Binomial distribution")

cNedValue nedf_binomial(cComponent *contextComponent, cNedValue argv[], int argc)
{
    int rng = argc == 3 ? (int)argv[2] : 0;
    return (intpar_t)contextComponent->binomial((int)argv[0], (double)argv[1], rng);
}

DEF(nedf_geometric,
    "int geometric(double p, int rng?)",
    "random/discrete",
    "Returns a random number from the Geometric distribution")

cNedValue nedf_geometric(cComponent *contextComponent, cNedValue argv[], int argc)
{
    int rng = argc == 2 ? (int)argv[1] : 0;
    return (intpar_t)contextComponent->geometric((double)argv[0], rng);
}

DEF(nedf_negbinomial,
    "int negbinomial(int n, double p, int rng?)",
    "random/discrete",
    "Returns a random number from the Negbinomial distribution")

cNedValue nedf_negbinomial(cComponent *contextComponent, cNedValue argv[], int argc)
{
    int rng = argc == 3 ? (int)argv[2] : 0;
    return (intpar_t)contextComponent->negbinomial((int)argv[0], (double)argv[1], rng);
}

DEF(nedf_poisson,
    "int poisson(double lambda, int rng?)",
    "random/discrete",
    "Returns a random number from the Poisson distribution")

cNedValue nedf_poisson(cComponent *contextComponent, cNedValue argv[], int argc)
{
    int rng = argc == 2 ? (int)argv[1] : 0;
    return (intpar_t)contextComponent->poisson((double)argv[0], rng);
}

//
// XML functions
//

DEF(nedf_xmldoc,
    "xml xmldoc(string filename, string xpath?)",
    "xml",
    "Parses the given XML file into a cXMLElement tree, and returns the root element. "
    "When called with two arguments, it returns the first element from the tree that "
    "matches the expression given in simplified XPath syntax.")

cNedValue nedf_xmldoc(cComponent *contextComponent, cNedValue argv[], int argc)
{
    const char *filename = argv[0].stringValue();
    const char *xpath = argc == 1 ? nullptr : argv[1].stringValue();
    cXMLElement *node = getEnvir()->getXMLDocument(filename, xpath);
    if (!node) {
        if (argc == 1)
            throw cRuntimeError("xmldoc(\"%s\"): Element not found", filename);
        else
            throw cRuntimeError("xmldoc(\"%s\", \"%s\"): Element not found", filename, xpath);
    }
    return node;
}

DEF(nedf_xml,
    "xml xml(string xmlstring, string xpath?)",
    "xml",
    "Parses the given XML string into a cXMLElement tree, and returns the root element. "
    "When called with two arguments, it returns the first element from the tree that "
    "matches the expression given in simplified XPath syntax.")

cNedValue nedf_xml(cComponent *contextComponent, cNedValue argv[], int argc)
{
    cXMLElement *node;
    if (argc == 1) {
        node = getEnvir()->getParsedXMLString(argv[0].stdstringValue().c_str(), nullptr);
        if (!node)
            throw cRuntimeError("xml(\"%s\"): Element not found", argv[0].stdstringValue().c_str());
    }
    else {
        node = getEnvir()->getParsedXMLString(argv[0].stdstringValue().c_str(), argv[1].stdstringValue().c_str());
        if (!node)
            throw cRuntimeError("xml(\"%s\", \"%s\"): Element not found", argv[0].stdstringValue().c_str(), argv[1].stdstringValue().c_str());
    }
    return node;
}


//
// misc utility functions
//

DEF(nedf_simTime,
    "quantity simTime()",
    "misc",
    "Returns the current simulation time.")

cNedValue nedf_simTime(cComponent *contextComponent, cNedValue argv[], int argc)
{
    return cNedValue(SIMTIME_DBL(simTime()), "s");
}

DEF(nedf_select,
    "any select(int index, ...)",
    "misc",
    "Returns the <index>th item from the rest of the argument list; numbering starts from 0.")

cNedValue nedf_select(cComponent *contextComponent, cNedValue argv[], int argc)
{
    long index = argv[0];
    if (index < 0)
        throw cRuntimeError("select(): Negative index %ld", index);
    if (index >= argc-1)
        throw cRuntimeError("select(): Index=%ld is too large", index, argc-1);
    return argv[index+1];
}

DEF(nedf_firstAvailable,
    "string firstAvailable(...)",
    "misc",
    "Accepts any number of strings, interprets them as NED type names "
    "(qualified or unqualified), and returns the first one that exists and "
    "its C++ implementation class is also available. Throws an error if "
    "none of the types are available.")

cNedValue nedf_firstAvailable(cComponent *contextComponent, cNedValue argv[], int argc)
{
    cRegistrationList *types = componentTypes.getInstance();
    for (int i = 0; i < argc; i++) {
        if (argv[i].getType() != cNedValue::STRING)
            throw cRuntimeError("firstAvailable(): String arguments expected");
        const char *name = argv[i].stringValue();
        cComponentType *c;
        c = dynamic_cast<cComponentType *>(types->lookup(name));  // by qualified name
        if (c && c->isAvailable())
            return argv[i];
        c = dynamic_cast<cComponentType *>(types->find(name));  // by simple name
        if (c && c->isAvailable())
            return argv[i];
    }

    std::string typelist;
    for (int i = 0; i < argc; i++)
        typelist += std::string(i == 0 ? "" : ", ") + argv[i].stdstringValue();
    throw cRuntimeError("None of the following NED types are available: %s", typelist.c_str());
}

}  // namespace omnetpp

