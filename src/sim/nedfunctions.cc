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
#include <fstream>
#include <functional>
#include "common/expression.h"
#include "common/stringutil.h"
#include "common/opp_ctype.h"
#include "common/unitconversion.h"
#include "common/fileutil.h"
#include "common/stringutil.h"
#include "common/stringtokenizer.h"
#include "omnetpp/distrib.h"
#include "omnetpp/cnedmathfunction.h"
#include "omnetpp/cnedfunction.h"
#include "omnetpp/cexception.h"
#include "omnetpp/cstringtokenizer.h"
#include "omnetpp/cconfiguration.h"
#include "omnetpp/ccomponenttype.h"
#include "omnetpp/csimulation.h"
#include "omnetpp/cmodule.h"
#include "omnetpp/cxmlelement.h"
#include "omnetpp/cvaluearray.h"
#include "omnetpp/cvaluemap.h"

using namespace omnetpp::common;
using namespace omnetpp::internal;

namespace omnetpp {

void nedfunctions_dummy() {} //see util.cc

#define DEF(FUNCTION, SIGNATURE, CATEGORY, DESCRIPTION) \
        cValue FUNCTION(cComponent *context, cValue argv[], int argc); \
        Define_NED_Function2(FUNCTION, SIGNATURE, CATEGORY, DESCRIPTION);

#define DEF2(FUNCTION, SIGNATURE, CATEGORY, DESCRIPTION) \
        cValue FUNCTION(cExpression::Context *context, cValue argv[], int argc); \
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

Define_NED_Math_Function3(ceil, 1,  "math", "Rounds up; see standard C function of the same name")
Define_NED_Math_Function3(floor, 1, "math", "Rounds down; see standard C function of the same name")

Define_NED_Math_Function3(exp, 1,   "math", "Exponential; see standard C function of the same name")
Define_NED_Math_Function3(pow, 2,   "math", "Power; see standard C function of the same name")
Define_NED_Math_Function3(sqrt, 1,  "math", "Square root; see standard C function of the same name")

Define_NED_Math_Function3(hypot, 2, "math", "Length of the hypotenuse; see standard C function of the same name")

Define_NED_Math_Function3(log, 1,   "math", "Natural logarithm; see standard C function of the same name")
Define_NED_Math_Function3(log10, 1, "math", "Base-10 logarithm; see standard C function of the same name")


DEF(nedf_fabs,
    "quantity fabs(quantity x)",
    "math",
    "Returns the absolute value of the quantity.")

cValue nedf_fabs(cComponent *contextComponent, cValue argv[], int argc)
{
    return cValue(fabs(argv[0].doubleValueRaw()), argv[0].getUnit());
}

DEF(nedf_fmod,
    "quantity fmod(quantity x, quantity y)",
    "math",
    "Returns the floating-point remainder of x/y; unit conversion takes place if needed.")

cValue nedf_fmod(cComponent *contextComponent, cValue argv[], int argc)
{
    double argv1converted = argv[1].doubleValueInUnit(argv[0].getUnit());
    return cValue(fmod(argv[0].doubleValueRaw(), argv1converted), argv[0].getUnit());
}

DEF(nedf_min,
    "quantity min(quantity a, quantity b)",
    "math",
    "Returns the smaller one of the two quantities; unit conversion takes place if needed.")
cValue nedf_min(cComponent *contextComponent, cValue argv[], int argc)
{
    double argv1converted = argv[1].doubleValueInUnit(argv[0].getUnit());
    return argv[0].doubleValueRaw() < argv1converted ? argv[0] : argv[1];
}

DEF(nedf_max,
    "quantity max(quantity a, quantity b)",
    "math",
    "Returns the greater one of the two quantities; unit conversion takes place if needed.")

cValue nedf_max(cComponent *contextComponent, cValue argv[], int argc)
{
    double argv1converted = argv[1].doubleValueInUnit(argv[0].getUnit());
    return argv[0].doubleValueRaw() < argv1converted ? argv[1] : argv[0];
}


//
// Unit handling
//

DEF(nedf_dropUnit,
    "double dropUnit(quantity x)",
    "units",
    "Removes the unit of measurement from quantity x.")

cValue nedf_dropUnit(cComponent *contextComponent, cValue argv[], int argc)
{
    argv[0].setUnit(nullptr);
    return argv[0];
}

DEF(nedf_replaceUnit,
    "quantity replaceUnit(quantity x, string unit)",
    "units",
    "Replaces the unit of x with the given unit.")

cValue nedf_replaceUnit(cComponent *contextComponent, cValue argv[], int argc)
{
    const char *newUnit = (const char *)argv[1];
    argv[0].setUnit(newUnit);
    return argv[0];
}

DEF(nedf_convertUnit,
    "quantity convertUnit(quantity x, string unit)",
    "units",
    "Converts x to the given unit.")

cValue nedf_convertUnit(cComponent *contextComponent, cValue argv[], int argc)
{
    const char *newUnit = (const char *)argv[1];
    argv[0].convertTo(newUnit);
    return argv[0];
}

DEF(nedf_unitOf,
    "string unitOf(quantity x)",
    "units",
    "Returns the unit of the given quantity.")

cValue nedf_unitOf(cComponent *contextComponent, cValue argv[], int argc)
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

cValue nedf_length(cComponent *contextComponent, cValue argv[], int argc)
{
    return (intval_t)argv[0].stdstringValue().size();
}

DEF(nedf_contains,
    "bool contains(string s, string substr)",
    "strings",
    "Returns true if string s contains substr as substring")

cValue nedf_contains(cComponent *contextComponent, cValue argv[], int argc)
{
    return argv[0].stdstringValue().find(argv[1].stdstringValue()) != std::string::npos;
}

DEF(nedf_substring,
    "string substring(string s, int pos, int len?)",
    "strings",
    "Return the substring of s starting at the given position, either to the end of the string or maximum len characters")

cValue nedf_substring(cComponent *contextComponent, cValue argv[], int argc)
{
    int size = argv[0].stdstringValue().size();
    int index = (int)argv[1];
    int length = argc == 3 ? (int)argv[2] : size - index;

    if (index < 0 || index > size)
        throw cRuntimeError("Index out of bounds");
    if (length < 0)
        throw cRuntimeError("Length is negative");
    return argv[0].stdstringValue().substr(index, length);
}

DEF(nedf_substringBefore,
    "string substringBefore(string s, string substr)",
    "strings",
    "Returns the substring of s before the first occurrence of substr, or the empty string if s does not contain substr.")

cValue nedf_substringBefore(cComponent *contextComponent, cValue argv[], int argc)
{
    size_t pos = argv[0].stdstringValue().find(argv[1].stdstringValue());
    return pos == std::string::npos ? "" : argv[0].stdstringValue().substr(0, pos);
}

DEF(nedf_substringAfter,
    "string substringAfter(string s, string substr)",
    "strings",
    "Returns the substring of s after the first occurrence of substr, or the empty string if s does not contain substr.")

cValue nedf_substringAfter(cComponent *contextComponent, cValue argv[], int argc)
{
    size_t pos = argv[0].stdstringValue().find(argv[1].stdstringValue());
    return pos == std::string::npos ? "" : argv[0].stdstringValue().substr(pos + argv[1].stdstringValue().size());
}

DEF(nedf_substringBeforeLast,
    "string substringBeforeLast(string s, string substr)",
    "strings",
    "Returns the substring of s before the last occurrence of substr, or the empty string if s does not contain substr.")

cValue nedf_substringBeforeLast(cComponent *contextComponent, cValue argv[], int argc)
{
    size_t pos = argv[0].stdstringValue().rfind(argv[1].stdstringValue());
    return pos == std::string::npos ? "" : argv[0].stdstringValue().substr(0, pos);
}

DEF(nedf_substringAfterLast,
    "string substringAfterLast(string s, string substr)",
    "strings",
    "Returns the substring of s after the last occurrence of substr, or the empty string if s does not contain substr.")

cValue nedf_substringAfterLast(cComponent *contextComponent, cValue argv[], int argc)
{
    size_t pos = argv[0].stdstringValue().rfind(argv[1].stdstringValue());
    return pos == std::string::npos ? "" : argv[0].stdstringValue().substr(pos + argv[1].stdstringValue().size());
}

DEF(nedf_startsWith,
    "bool startsWith(string s, string substr)",
    "strings",
    "Returns true if s begins with the substring substr.")

cValue nedf_startsWith(cComponent *contextComponent, cValue argv[], int argc)
{
    return argv[0].stdstringValue().find(argv[1].stdstringValue()) == 0;
}

DEF(nedf_endsWith,
    "bool endsWith(string s, string substr)",
    "strings",
    "Returns true if s ends with the substring substr.")

cValue nedf_endsWith(cComponent *contextComponent, cValue argv[], int argc)
{
    return argv[0].stdstringValue().rfind(argv[1].stdstringValue()) == argv[0].stdstringValue().size() - argv[1].stdstringValue().size();
}

DEF(nedf_tail,
    "string tail(string s, int len)",
    "strings",
    "Returns the last len character of s, or the full s if it is shorter than len characters.")

cValue nedf_tail(cComponent *contextComponent, cValue argv[], int argc)
{
    int length = (int)argv[1];
    if (length < 0)
        throw cRuntimeError("Length is negative");
    int size = (int)argv[0].stdstringValue().size();
    return argv[0].stdstringValue().substr(std::max(0, size - length), size);
}

DEF(nedf_replace,
    "string replace(string s, string substr, string repl, int startPos?)",
    "strings",
    "Replaces all occurrences of substr in s with the string repl. If startPos is given, search begins from position startPos in s.")

cValue nedf_replace(cComponent *contextComponent, cValue argv[], int argc)
{
    std::string str = argv[0].stdstringValue();
    const std::string& search = argv[1].stdstringValue();
    const std::string& replacement = argv[2].stdstringValue();
    int index = 0;
    if (argc == 4) {
        index = (int)argv[3];
        if (index < 0)
            throw cRuntimeError("Start index is negative");
        if (index > (int)str.size())
            throw cRuntimeError("Start index out of bounds");
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

cValue nedf_replaceFirst(cComponent *contextComponent, cValue argv[], int argc)
{
    std::string str = argv[0].stdstringValue();
    const std::string& search = argv[1].stdstringValue();
    const std::string& replacement = argv[2].stdstringValue();
    int index = 0;
    if (argc == 4) {
        index = (int)argv[3];
        if (index < 0)
            throw cRuntimeError("Start index is negative");
        if (index > (int)str.size())
            throw cRuntimeError("Start index out of bounds");
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

cValue nedf_trim(cComponent *contextComponent, cValue argv[], int argc)
{
    return opp_trim(argv[0].stdstringValue());
}

DEF(nedf_indexOf,
    "int indexOf(string s, string substr)",
    "strings",
    "Returns the position of the first occurrence of substring substr in s, or -1 if s does not contain substr.")

cValue nedf_indexOf(cComponent *contextComponent, cValue argv[], int argc)
{
    return (intval_t)argv[0].stdstringValue().find(argv[1].stdstringValue());
}

DEF(nedf_choose,
    "string choose(int index, string list)",
    "strings",
    "Interprets list as a space-separated list, and returns the item at the given index. Negative and out-of-bounds indices cause an error.")

cValue nedf_choose(cComponent *contextComponent, cValue argv[], int argc)
{
    int index = (int)argv[0];
    if (index < 0)
        throw cRuntimeError("Negative index");
    cStringTokenizer tokenizer(argv[1].stdstringValue().c_str());
    for (int i = 0; i < index && tokenizer.hasMoreTokens(); i++)
        tokenizer.nextToken();
    if (!tokenizer.hasMoreTokens())
        throw cRuntimeError("Index out of bounds: %d", index);
    return tokenizer.nextToken();
}

DEF(nedf_toUpper,
    "string toUpper(string s)",
    "strings",
    "Converts s to all uppercase, and returns the result.")

cValue nedf_toUpper(cComponent *contextComponent, cValue argv[], int argc)
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

cValue nedf_toLower(cComponent *contextComponent, cValue argv[], int argc)
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

cValue nedf_expand(cComponent *contextComponent, cValue argv[], int argc)
{
    std::string tmp = argv[0].stdstringValue();
    tmp = getEnvir()->getConfig()->substituteVariables(tmp.c_str());
    return tmp;
}

DEF(nedf_bool,
    "bool bool(any x)",
    "conversion",
    "Converts x to bool, and returns the result. For numeric values, 0 and nan become false and other values become true; for strings, \"true\" becomes true and everything else becomes false.")

cValue nedf_bool(cComponent *contextComponent, cValue argv[], int argc)
{
    switch (argv[0].getType()) {
        case cValue::BOOL:
            return argv[0];
        case cValue::INT:
            return argv[0].intValue() != 0;
        case cValue::DOUBLE:
            return !std::isnan(argv[0].doubleValue()) && argv[0].doubleValue() != 0;
        case cValue::STRING:
            return argv[0].stdstringValue() == "true";
        case cValue::POINTER:
            throw cRuntimeError("Cannot convert object to bool");
        case cValue::UNDEF:
            throw cRuntimeError("Cannot convert 'undefined' to bool");
        default:
            throw cRuntimeError("Internal error: Invalid cValue type");
    }
}

DEF(nedf_int,
    "intquantity int(any x)",
    "conversion",
    "Converts x to int, and returns the result. A boolean argument becomes 0 or 1; a double is converted using floor(); a string is interpreted as number; an object argument causes an error. Units are preserved.")

cValue nedf_int(cComponent *contextComponent, cValue argv[], int argc)
{
    switch (argv[0].getType()) {
        case cValue::BOOL:
            return (intval_t)( (bool)argv[0] ? 1 : 0 );
        case cValue::INT:
            return argv[0];
        case cValue::DOUBLE:
            return cValue(checked_int_cast<intval_t>(floor(argv[0].doubleValueRaw())), argv[0].getUnit());
        case cValue::STRING: {
            std::string unit;
            double d = UnitConversion::parseQuantity(argv[0].stringValue(), unit);
            return cValue(checked_int_cast<intval_t>(floor(d)), unit.c_str());
        }
        case cValue::POINTER:
            throw cRuntimeError("Cannot convert object to int");
        case cValue::UNDEF:
            throw cRuntimeError("Cannot convert 'undefined' to int");
        default:
            throw cRuntimeError("Internal error: Invalid cValue type");
    }
}

DEF(nedf_double,
    "quantity double(any x)",
    "conversion",
    "Converts x to double, and returns the result. A boolean argument becomes 0 or 1; a string is interpreted as number; an object argument causes an error. Units are preserved.")

cValue nedf_double(cComponent *contextComponent, cValue argv[], int argc)
{
    switch (argv[0].getType()) {
        case cValue::BOOL:
            return (bool)argv[0] ? 1.0 : 0.0;
        case cValue::INT:
            return cValue(argv[0].doubleValueRaw(), argv[0].getUnit());
        case cValue::DOUBLE:
            return argv[0];
        case cValue::STRING: {
            std::string unit;
            double d = UnitConversion::parseQuantity(argv[0].stringValue(), unit);
            return cValue(d, unit.c_str());
        }
        case cValue::POINTER:
            throw cRuntimeError("Cannot convert object to double");
        case cValue::UNDEF:
            throw cRuntimeError("Cannot convert 'undefined' to double");
        default:
            throw cRuntimeError("Internal error: Invalid cValue type");
    }
}

DEF(nedf_string,
    "string string(any x)",
    "conversion",
    "Converts x to string, and returns the result.")

cValue nedf_string(cComponent *contextComponent, cValue argv[], int argc)
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

cValue nedf_fullPath(cComponent *contextComponent, cValue argv[], int argc)
{
    if (!contextComponent)
        throw cRuntimeError("No context component");
    return contextComponent->getFullPath();
}

DEF(nedf_fullName,
    "string fullName()",
    "ned",
    "Returns the full name of the module or channel in context.")

cValue nedf_fullName(cComponent *contextComponent, cValue argv[], int argc)
{
    if (!contextComponent)
        throw cRuntimeError("No context component");
    return contextComponent->getFullName();
}

DEF(nedf_parentIndex,
    "int parentIndex()",
    "ned",
    "Returns the index of the parent module, which has to be part of module vector.")

cValue nedf_parentIndex(cComponent *contextComponent, cValue argv[], int argc)
{
    if (!contextComponent)
        throw cRuntimeError("No context component");
    cModule *mod = contextComponent->getParentModule();
    if (!mod)
        throw cRuntimeError("'%s' has no parent module", contextComponent->getFullPath().c_str());
    if (!mod->isVector())
        throw cRuntimeError("Module '%s' is not a vector", mod->getFullPath().c_str());
    return (intval_t)mod->getIndex();
}

DEF(nedf_ancestorIndex,
    "int ancestorIndex(int numLevels)",
    "ned",
    "Returns the index of the ancestor module numLevels levels above the module or channel in context.")

cValue nedf_ancestorIndex(cComponent *contextComponent, cValue argv[], int argc)
{
    if (!contextComponent)
        throw cRuntimeError("No context component");
    int levels = (int)argv[0];
    if (levels < 0)
        throw cRuntimeError("Negative number of levels");
    if (levels == 0 && !contextComponent->isModule())
        throw cRuntimeError("numlevels==0 and this is not a module");
    cModule *mod = dynamic_cast<cModule *>(contextComponent);
    for (int i = 0; mod && i < levels; i++)
        mod = mod->getParentModule();
    if (!mod)
        throw cRuntimeError("Argument is larger than current nesting level");
    if (!mod->isVector())
        throw cRuntimeError("Module '%s' is not a vector", mod->getFullPath().c_str());
    return (intval_t)mod->getIndex();
}


//
// Random variate generation.
//

// continuous
DEF(nedf_uniform,
    "quantity uniform(quantity a, quantity b, int rng?)",
    "random/continuous",
    "Returns a random number from the Uniform distribution")

cValue nedf_uniform(cComponent *contextComponent, cValue argv[], int argc)
{
    if (!contextComponent)
        throw cRuntimeError("No context component");
    int rng = argc == 3 ? (int)argv[2] : 0;
    double argv1converted = argv[1].doubleValueInUnit(argv[0].getUnit());
    return cValue(contextComponent->uniform(argv[0].doubleValueRaw(), argv1converted, rng), argv[0].getUnit());
}

DEF(nedf_exponential,
    "quantity exponential(quantity mean, int rng?)",
    "random/continuous",
    "Returns a random number from the Exponential distribution")

cValue nedf_exponential(cComponent *contextComponent, cValue argv[], int argc)
{
    if (!contextComponent)
        throw cRuntimeError("No context component");
    int rng = argc == 2 ? (int)argv[1] : 0;
    return cValue(contextComponent->exponential(argv[0].doubleValueRaw(), rng), argv[0].getUnit());
}

DEF(nedf_normal,
    "quantity normal(quantity mean, quantity stddev, int rng?)",
    "random/continuous",
    "Returns a random number from the Normal distribution")

cValue nedf_normal(cComponent *contextComponent, cValue argv[], int argc)
{
    if (!contextComponent)
        throw cRuntimeError("No context component");
    int rng = argc == 3 ? (int)argv[2] : 0;
    double argv1converted = argv[1].doubleValueInUnit(argv[0].getUnit());
    return cValue(contextComponent->normal(argv[0].doubleValueRaw(), argv1converted, rng), argv[0].getUnit());
}

DEF(nedf_truncnormal,
    "quantity truncnormal(quantity mean, quantity stddev, int rng?)",
    "random/continuous",
    "Returns a random number from the truncated Normal distribution")

cValue nedf_truncnormal(cComponent *contextComponent, cValue argv[], int argc)
{
    if (!contextComponent)
        throw cRuntimeError("No context component");
    int rng = argc == 3 ? (int)argv[2] : 0;
    double argv1converted = argv[1].doubleValueInUnit(argv[0].getUnit());
    return cValue(contextComponent->truncnormal(argv[0].doubleValueRaw(), argv1converted, rng), argv[0].getUnit());
}

DEF(nedf_gamma_d,
    "quantity gamma_d(double alpha, quantity theta, int rng?)",
    "random/continuous",
    "Returns a random number from the Gamma distribution")

cValue nedf_gamma_d(cComponent *contextComponent, cValue argv[], int argc)
{
    if (!contextComponent)
        throw cRuntimeError("No context component");
    int rng = argc == 3 ? (int)argv[2] : 0;
    return cValue(contextComponent->gamma_d((double)argv[0], argv[1].doubleValueRaw(), rng), argv[1].getUnit());
}

DEF(nedf_beta,
    "double beta(double alpha1, double alpha2, int rng?)",
    "random/continuous",
    "Returns a random number from the Beta distribution")

cValue nedf_beta(cComponent *contextComponent, cValue argv[], int argc)
{
    if (!contextComponent)
        throw cRuntimeError("No context component");
    int rng = argc == 3 ? (int)argv[2] : 0;
    argv[0] = contextComponent->beta((double)argv[0], (double)argv[1], rng);
    return argv[0];
}

DEF(nedf_erlang_k,
    "quantity erlang_k(int k, quantity mean, int rng?)",
    "random/continuous",
    "Returns a random number from the Erlang distribution")

cValue nedf_erlang_k(cComponent *contextComponent, cValue argv[], int argc)
{
    if (!contextComponent)
        throw cRuntimeError("No context component");
    int k = (int)argv[0];
    if (k < 0)
        throw cRuntimeError("k parameter (number of phases) must be positive (k=%d)", k);
    int rng = argc == 3 ? (int)argv[2] : 0;
    return cValue(contextComponent->erlang_k(k, argv[1].doubleValueRaw(), rng), argv[1].getUnit());
}

DEF(nedf_chi_square,
    "double chi_square(int k, int rng?)",
    "random/continuous",
    "Returns a random number from the Chi-square distribution")

cValue nedf_chi_square(cComponent *contextComponent, cValue argv[], int argc)
{
    if (!contextComponent)
        throw cRuntimeError("No context component");
    int k = (int)argv[0];
    if (k < 0)
        throw cRuntimeError("k parameter (degrees of freedom) must be positive (k=%d)", k);
    int rng = argc == 2 ? (int)argv[1] : 0;
    return contextComponent->chi_square(k, rng);
}

DEF(nedf_student_t,
    "double student_t(int i, int rng?)",
    "random/continuous",
    "Returns a random number from the Student-t distribution")

cValue nedf_student_t(cComponent *contextComponent, cValue argv[], int argc)
{
    if (!contextComponent)
        throw cRuntimeError("No context component");
    int i = (int)argv[0];
    if (i < 0)
        throw cRuntimeError("i parameter (degrees of freedom) must be positive (i=%d)", i);
    int rng = argc == 2 ? (int)argv[1] : 0;
    return contextComponent->student_t(i, rng);
}

DEF(nedf_cauchy,
    "quantity cauchy(quantity a, quantity b, int rng?)",
    "random/continuous",
    "Returns a random number from the Cauchy distribution")

cValue nedf_cauchy(cComponent *contextComponent, cValue argv[], int argc)
{
    if (!contextComponent)
        throw cRuntimeError("No context component");
    int rng = argc == 3 ? (int)argv[2] : 0;
    double argv1converted = argv[1].doubleValueInUnit(argv[0].getUnit());
    return cValue(contextComponent->cauchy(argv[0].doubleValueRaw(), argv1converted, rng), argv[0].getUnit());
}

DEF(nedf_triang,
    "quantity triang(quantity a, quantity b, quantity c, int rng?)",
    "random/continuous",
    "Returns a random number from the Triangular distribution")

cValue nedf_triang(cComponent *contextComponent, cValue argv[], int argc)
{
    if (!contextComponent)
        throw cRuntimeError("No context component");
    int rng = argc == 4 ? (int)argv[3] : 0;
    double argv1converted = argv[1].doubleValueInUnit(argv[0].getUnit());
    double argv2converted = argv[2].doubleValueInUnit(argv[0].getUnit());
    return cValue(contextComponent->triang(argv[0].doubleValueRaw(), argv1converted, argv2converted, rng), argv[0].getUnit());
}

DEF(nedf_lognormal,
    "double lognormal(double m, double w, int rng?)",
    "random/continuous",
    "Returns a random number from the Lognormal distribution")

cValue nedf_lognormal(cComponent *contextComponent, cValue argv[], int argc)
{
    if (!contextComponent)
        throw cRuntimeError("No context component");
    int rng = argc == 3 ? (int)argv[2] : 0;
    return contextComponent->lognormal((double)argv[0], (double)argv[1], rng);
}

DEF(nedf_weibull,
    "quantity weibull(quantity a, quantity b, int rng?)",
    "random/continuous",
    "Returns a random number from the Weibull distribution")

cValue nedf_weibull(cComponent *contextComponent, cValue argv[], int argc)
{
    if (!contextComponent)
        throw cRuntimeError("No context component");
    int rng = argc == 3 ? (int)argv[2] : 0;
    double argv1converted = argv[1].doubleValueInUnit(argv[0].getUnit());
    return cValue(contextComponent->weibull(argv[0].doubleValueRaw(), argv1converted, rng), argv[0].getUnit());
}

DEF(nedf_pareto_shifted,
    "quantity pareto_shifted(double a, quantity b, quantity c, int rng?)",
    "random/continuous",
    "Returns a random number from the Pareto-shifted distribution")

cValue nedf_pareto_shifted(cComponent *contextComponent, cValue argv[], int argc)
{
    if (!contextComponent)
        throw cRuntimeError("No context component");
    int rng = argc == 4 ? (int)argv[3] : 0;
    double argv2converted = argv[2].doubleValueInUnit(argv[1].getUnit());
    return cValue(contextComponent->pareto_shifted((double)argv[0], argv[1].doubleValueRaw(), argv2converted, rng), argv[1].getUnit());
}

// discrete

DEF(nedf_intuniform,
    "int intuniform(intquantity a, intquantity b, int rng?)",
    "random/discrete",
    "Returns a random integer uniformly distributed over [a,b]")

cValue nedf_intuniform(cComponent *contextComponent, cValue argv[], int argc)
{
    if (!contextComponent)
        throw cRuntimeError("No context component");
    int rng = argc == 3 ? (int)argv[2] : 0;
    if (opp_strcmp(argv[0].getUnit(), argv[1].getUnit()) != 0)
        throw cRuntimeError("Arguments must have the same unit, got (%s,%s)", argv[0].stdstringValue().c_str(), argv[1].stdstringValue().c_str());
    return cValue((intval_t)contextComponent->intuniform(argv[0].intValueRaw(), argv[1].intValueRaw(), rng), argv[1].getUnit());
}

DEF(nedf_intuniformexcl,
    "int intuniformexcl(intquantity a, intquantity b, int rng?)",
    "random/discrete",
    "Returns a random integer uniformly distributed over [a,b), that is, [a,b-1]")

cValue nedf_intuniformexcl(cComponent *contextComponent, cValue argv[], int argc)
{
    if (!contextComponent)
        throw cRuntimeError("No context component");
    int rng = argc == 3 ? (int)argv[2] : 0;
    if (opp_strcmp(argv[0].getUnit(), argv[1].getUnit()) != 0)
        throw cRuntimeError("Arguments must have the same unit, got (%s,%s)", argv[0].stdstringValue().c_str(), argv[1].stdstringValue().c_str());
    return cValue((intval_t)contextComponent->intuniformexcl(argv[0].intValueRaw(), argv[1].intValueRaw(), rng), argv[1].getUnit());
}

DEF(nedf_bernoulli,
    "int bernoulli(double p, int rng?)",
    "random/discrete",
    "Returns a random number from the Bernoulli distribution")

cValue nedf_bernoulli(cComponent *contextComponent, cValue argv[], int argc)
{
    if (!contextComponent)
        throw cRuntimeError("No context component");
    int rng = argc == 2 ? (int)argv[1] : 0;
    return (intval_t)contextComponent->bernoulli((double)argv[0], rng);
}

DEF(nedf_binomial,
    "int binomial(int n, double p, int rng?)",
    "random/discrete",
    "Returns a random number from the Binomial distribution")

cValue nedf_binomial(cComponent *contextComponent, cValue argv[], int argc)
{
    if (!contextComponent)
        throw cRuntimeError("No context component");
    int rng = argc == 3 ? (int)argv[2] : 0;
    return (intval_t)contextComponent->binomial((int)argv[0], (double)argv[1], rng);
}

DEF(nedf_geometric,
    "int geometric(double p, int rng?)",
    "random/discrete",
    "Returns a random number from the Geometric distribution")

cValue nedf_geometric(cComponent *contextComponent, cValue argv[], int argc)
{
    if (!contextComponent)
        throw cRuntimeError("No context component");
    int rng = argc == 2 ? (int)argv[1] : 0;
    return (intval_t)contextComponent->geometric((double)argv[0], rng);
}

DEF(nedf_negbinomial,
    "int negbinomial(int n, double p, int rng?)",
    "random/discrete",
    "Returns a random number from the Negbinomial distribution")

cValue nedf_negbinomial(cComponent *contextComponent, cValue argv[], int argc)
{
    if (!contextComponent)
        throw cRuntimeError("No context component");
    int rng = argc == 3 ? (int)argv[2] : 0;
    return (intval_t)contextComponent->negbinomial((int)argv[0], (double)argv[1], rng);
}

DEF(nedf_poisson,
    "int poisson(double lambda, int rng?)",
    "random/discrete",
    "Returns a random number from the Poisson distribution")

cValue nedf_poisson(cComponent *contextComponent, cValue argv[], int argc)
{
    if (!contextComponent)
        throw cRuntimeError("No context component");
    int rng = argc == 2 ? (int)argv[1] : 0;
    return (intval_t)contextComponent->poisson((double)argv[0], rng);
}

//
// Misc functions
//

static cValue opp_eval(const char *txt, cExpression::Context *context) //TOOD simutil?
{
    try {
        cDynamicExpression expr;
        expr.parseNedExpr(txt);
        return expr.evaluate(context);
    }
    catch (std::exception& e) {
        throw cRuntimeError("Error evaluating expression \"%s\": %s", txt, e.what());
    }
}

DEF2(nedf_eval,
    "any eval(string expr)",
    "misc",
    "Evaluates the NED expression in the call site's context.")

cValue nedf_eval(cExpression::Context *context, cValue argv[], int argc)
{
    return opp_eval(argv[0].stringValue(), context);
}


//
// I/O functions
//

DEF2(nedf_readXML,
    "xml readXML(string filename, string xpath?)",
    "i/o",
    "Parses the given XML file into a cXMLElement tree, and returns the root element. "
    "When called with two arguments, it returns the first element from the tree that "
    "matches the expression given in simplified XPath syntax.")

cValue nedf_readXML(cExpression::Context *context, cValue argv[], int argc)
{
    std::string filepath = concatDirAndFile(context->baseDirectory, argv[0].stringValue());
    const char *xpath = argc == 1 ? nullptr : argv[1].stringValue();
    cXMLElement *node = getEnvir()->getXMLDocument(filepath.c_str(), xpath);
    if (!node) {
        if (argc == 1)
            throw cRuntimeError("File \"%s\"not found", filepath.c_str());
        else
            throw cRuntimeError("Element \"%s\" in file \"%s\" not found", xpath, filepath.c_str());
    }
    return static_cast<cObject*>(node);
}

DEF2(nedf_parseXML,
    "xml parseXML(string xmlstring, string xpath?)",
    "i/o",
    "Parses the given XML string into a cXMLElement tree, and returns the root element. "
    "When called with two arguments, it returns the first element from the tree that "
    "matches the expression given in simplified XPath syntax.")

cValue nedf_parseXML(cExpression::Context *context, cValue argv[], int argc)
{
    const char *xmlText = argv[0].stringValue();
    const char *xpath = argc == 1 ? nullptr : argv[1].stringValue();
    cXMLElement *node = getEnvir()->getParsedXMLString(xmlText, xpath);
    if (!node) {
        if (argc == 1)
            throw cRuntimeError("Empty XML document");
        else
            throw cRuntimeError("Element \"%s\" not found in XML document string", xpath);
    }
    return static_cast<cObject*>(node);
}

DEF2(nedf_xmldoc,
    "xml xmldoc(string filename, string xpath?)",
    "xml",
    "Parses the given XML file into a cXMLElement tree, and returns the root element. "
    "When called with two arguments, it returns the first element from the tree that "
    "matches the expression given in simplified XPath syntax. "
    "Note: This is an alias to the readXML() function.")

cValue nedf_xmldoc(cExpression::Context *context, cValue argv[], int argc)
{
    return nedf_readXML(context, argv, argc);
}

DEF2(nedf_xml,
    "xml xml(string xmlstring, string xpath?)",
    "xml",
    "Parses the given XML string into a cXMLElement tree, and returns the root element. "
    "When called with two arguments, it returns the first element from the tree that "
    "matches the expression given in simplified XPath syntax. "
    "Note: This is an alias to the parseXML() function.")

cValue nedf_xml(cExpression::Context *context, cValue argv[], int argc)
{
    return nedf_parseXML(context, argv, argc);
}

DEF2(nedf_readFile,
    "string readFile(string filename)",
    "i/o",
    "Opens the specified text file, and returns its content as a string. "
    "If filename is a relative path, it is understood as relative to the "
    "location of the ini or NED file where the readFile() call occurs.")

cValue nedf_readFile(cExpression::Context *context, cValue argv[], int argc)
{
    std::string filepath = concatDirAndFile(context->baseDirectory, argv[0].stringValue());
    std::ifstream in(filepath.c_str());
    if (in.fail())
        throw cRuntimeError("Cannot open file \"%s\" for read", filepath.c_str());
    std::ostringstream sstr;
    sstr << in.rdbuf();
    if (in.bad())
        throw cRuntimeError("Could not read file \"%s\"", filepath.c_str());
    return sstr.str();
}

DEF2(nedf_workingDir,
    "string workingDir()",
    "i/o",
    "Returns the current working directory."
    "")
cValue nedf_workingDir(cExpression::Context *context, cValue argv[], int argc)
{
    return getWorkingDir();
}

DEF2(nedf_baseDir,
    "string baseDir()",
    "i/o",
    "Returns the absolute filesystem path to the directory of the ini or NED file "
    "where the baseDir() call occurs.")

cValue nedf_baseDir(cExpression::Context *context, cValue argv[], int argc)
{
    return context->baseDirectory;
}

DEF2(nedf_resolveFile,
    "string resolveFile(string directory, string filename)",
    "i/o",
    "Joins its arguments as file paths, except that when the second argument is an "
    "absolute filesystem path, it is returned unchanged. This is purely a string "
    "operation; neither directory nor filename needs to exist in the file system.")

cValue nedf_resolveFile(cExpression::Context *context, cValue argv[], int argc)
{
    return concatDirAndFile(argv[0].stringValue(), argv[1].stringValue());
}

DEF2(nedf_absFilePath,
    "string absFilePath(string filename)",
    "i/o",
    "Converts filename to an absolute filesystem path. Absolute paths are returned "
    "unchanged; relative paths are understood as relative to the location of the ini "
    "or NED file where the call occurs (see baseDir()).")

cValue nedf_absFilePath(cExpression::Context *context, cValue argv[], int argc)
{
    return concatDirAndFile(context->baseDirectory, argv[0].stringValue());
}

static cValue parseCSVCell(const char *s)
{
    // assume trimmed items
    ASSERT(!opp_isspace(*s));

    // empty -> ""
    if (opp_isempty(s))
        return s;

    // try as boolean
    if (strcmp(s, "true") == 0)
        return true;
    if (strcmp(s, "false") == 0)
        return false;

    // try as quoted string
    if (*s == '"' || *s == '\'') {
        try {
            return opp_parsequotedstr(s, *s);
        }
        catch (std::exception&) {}
        return s; // fallback: verbatim string
    }

    // try as integer
    try {
        char *endp;
        intval_t l = opp_strtoll(s, &endp);
        if (opp_isblank(endp))
            return l;
    }
    catch (std::exception&) {}

    // try as double
    try {
        char *endp;
        double d = opp_strtod(s, &endp);
        if (opp_isblank(endp))
            return d;
    }
    catch (std::exception&) {}

    return s; // fallback: verbatim string
}

DEF2(nedf_parseCSV,
    "any parseCSV(string str)",
    "i/o",
    "Parses the given string as a comma-separated CSV, and returns it as an array of "
    "arrays. Elements can be boolean (true/false), numeric (integer or double, "
    "with or without measurement unit), quoted string, or unquoted string. "
    "Items that cannot be parsed as any of the more specific types are interpreted "
    "as unquoted strings. "
    "See readCSV() for details of the the accepted CSV flavor.")

cValue nedf_parseCSV(cExpression::Context *context, cValue argv[], int argc)
{
    const char *str = argv[0].stringValue();
    cValueArray *rows = new cValueArray();
    int rowNum = 0;
    for (std::string line : opp_split(str, "\n")) {
        if (opp_isblank(line.c_str()) || line.c_str()[0] == '#')
            continue;
        cValueArray *row = new cValueArray(opp_stringf("row%d", ++rowNum).c_str());
        StringTokenizer tokenizer(line.c_str(), ",", StringTokenizer::HONOR_QUOTES | StringTokenizer::KEEP_EMPTY);
        while (tokenizer.hasMoreTokens()) {
            const char *item = tokenizer.nextToken();
            row->add(parseCSVCell(item));
        }
        rows->add(cValue(row));
    }
    return rows;
}

DEF2(nedf_parseExtendedCSV,
    "any parseExtendedCSV(string str)",
    "i/o",
    "Parses the given string as a comma-separated CSV, and returns it as "
    "an array of arrays. Elements are parsed as NED expressions, and are "
    "evaluated in the caller's context. "
    "See readCSV() for details of the the accepted CSV flavor.")

cValue nedf_parseExtendedCSV(cExpression::Context *context, cValue argv[], int argc)
{
    const char *str = argv[0].stringValue();
    cValueArray *rows = new cValueArray();
    int rowNum = 0;
    for (std::string line : opp_split(str, "\n")) {
        if (opp_isblank(line.c_str()) || line.c_str()[0] == '#')
            continue;
        cValueArray *row = new cValueArray(opp_stringf("row%d", ++rowNum).c_str());
        StringTokenizer tokenizer(line.c_str(), ",", StringTokenizer::HONOR_QUOTES | StringTokenizer::HONOR_PARENS | StringTokenizer::KEEP_EMPTY);
        while (tokenizer.hasMoreTokens()) {
            const char *item = tokenizer.nextToken();
            row->add(opp_eval(item, context));
        }
        rows->add(cValue(row));
    }
    return rows;
}

DEF2(nedf_readCSV,
    "any readCSV(string filename)",
    "i/o",
    "Parses the content of the given text file as comma-separated CSV, and returns it "
    "as an array of arrays. Elements can be boolean (`true` or `false`), numeric (integer "
    "or double, with or without measurement unit), quoted string, or unquoted string. "
    "Items that cannot be parsed as any of the more specific types are interpreted as "
    "unquoted strings. "
    "CSV parsing rules: separator is comma; "
    "blank (whitespace-only) lines are ignored; "
    "lines that contain hash mark `#` on column 1 are considered comments and are ignored; "
    "items are trimmed of leading and trailing whitespace before processing; "
    "no line continuation with backslash; "
    "quoted strings may be delimited with single or double quotes; "
    "quoted strings may contain C-like backslash escapes; "
    "no support for splitting strings over multiple lines; "
    "no special treatment for the first (possibly header) line.")

cValue nedf_readCSV(cExpression::Context *context, cValue argv[], int argc)
{
    argv[0] = nedf_readFile(context, argv, argc);
    return nedf_parseCSV(context, argv, argc);
}

DEF2(nedf_readExtendedCSV,
    "any readExtendedCSV(string filename)",
    "i/o",
    "Parses the content of the given text file as comma-separated CSV, and returns it "
    "as an array of arrays. Elements are parsed as NED expressions, and are evaluated "
    "in the caller's context. See readCSV() for details of the the accepted CSV flavor.")

cValue nedf_readExtendedCSV(cExpression::Context *context, cValue argv[], int argc)
{
    argv[0] = nedf_readFile(context, argv, argc);
    return nedf_parseExtendedCSV(context, argv, argc);
}

DEF2(nedf_parseJSON,
    "any parseJSON(string str)",
    "i/o",
    "Parses the given string as JSON, and returns its contents. The syntax is more "
    "permissive than standard JSON: it additionally allows the special numeric values "
    "`nan`, `inf` and `-inf`, the use of measurement units, and object keys are also "
    "accepted without quotation marks if it doesn't interfere with parsing.")

cValue nedf_parseJSON(cExpression::Context *context, cValue argv[], int argc)
{
    // validate that input only contains constructs valid in JSON
    Expression expr;
    using AstNode = Expression::AstNode;
    AstNode *tree = expr.parseToAst(argv[0].stringValue());

    std::function<void(AstNode *node)> check;
    check = [&](AstNode *node) {
        auto type = node->type;
        if (type != AstNode::CONSTANT && type != AstNode::ARRAY && type != AstNode::OBJECT && type != AstNode::KEYVALUE)
            throw cRuntimeError("Illegal construct in JSON: %s", node->str().c_str());
        if (type == AstNode::OBJECT && !node->name.empty())
            throw cRuntimeError("Tagged objects are not allowed in JSON: '%s {...}'", node->name.c_str());
        for (AstNode *child : node->children)
            check(child);
    };
    check(tree);

    delete tree;

    return nedf_eval(context, argv, argc);
}

DEF2(nedf_readJSON,
    "any readJSON(string filename)",
    "i/o",
    "Parses the given text file as JSON, and returns its contents. The syntax is more "
    "permissive than standard JSON: it additionally allows the special numeric values "
    "`nan`, `inf` and `-inf`, the use of measurement units, and object keys are also "
    "accepted without quotation marks if it doesn't interfere with parsing.")

cValue nedf_readJSON(cExpression::Context *context, cValue argv[], int argc)
{
    argv[0] = nedf_readFile(context, argv, argc);
    return nedf_parseJSON(context, argv, argc);
}

DEF2(nedf_parseExtendedJSON,
    "any parseExtendedJSON(string str)",
    "i/o",
    "Parses the given string as Extended JSON, and returns its contents. Extended JSON "
    "allows any value to be a valid NED expression (instead of just constants allowed "
    "by strict JSON), and some extensions to the object syntax. Actually, as the "
    "NED expression syntax includes JSON-like arrays and objects, \"parsing\" is done "
    "simply by evaluating the string as a NED expression in the caller's context.")

cValue nedf_parseExtendedJSON(cExpression::Context *context, cValue argv[], int argc)
{
    return opp_eval(argv[0].stringValue(), context);
}

DEF2(nedf_readExtendedJSON,
    "any readExtendedJSON(string filename)",
    "i/o",
    "Parses the given text file as Extended JSON, and returns its contents. Extended JSON "
    "allows any value to be a valid NED expression (instead of just constants allowed "
    "by strict JSON), and some extensions to the object syntax. Actually, as the "
    "NED expression syntax includes JSON-like arrays and objects, \"parsing\" is done "
    "simply by evaluating the content as a NED expression in the caller's context.")

cValue nedf_readExtendedJSON(cExpression::Context *context, cValue argv[], int argc)
{
    argv[0] = nedf_readFile(context, argv, argc);
    return nedf_parseExtendedJSON(context, argv, argc);
}

//
// misc utility functions
//

DEF(nedf_simTime,
    "quantity simTime()",
    "misc",
    "Returns the current simulation time.")

cValue nedf_simTime(cComponent *contextComponent, cValue argv[], int argc)
{
    return cValue(SIMTIME_DBL(simTime()), "s");
}

DEF(nedf_select,
    "any select(int index, ...)",
    "misc",
    "Returns the <index>th item from the rest of the argument list; numbering starts from 0.")

cValue nedf_select(cComponent *contextComponent, cValue argv[], int argc)
{
    long index = argv[0];
    if (index < 0)
        throw cRuntimeError("Negative index %ld", index);
    if (index >= argc-1)
        throw cRuntimeError("Index=%ld is too large (max: %d)", index, argc-2);
    return argv[index+1];
}

DEF(nedf_firstAvailable,
    "string firstAvailable(...)",
    "misc",
    "Accepts any number of strings, interprets them as NED type names "
    "(qualified or unqualified), and returns the first one that exists and "
    "its C++ implementation class is also available. Throws an error if "
    "none of the types are available.")

cValue nedf_firstAvailable(cComponent *contextComponent, cValue argv[], int argc)
{
    cRegistrationList *types = componentTypes.getInstance();
    for (int i = 0; i < argc; i++) {
        if (argv[i].getType() != cValue::STRING)
            throw cRuntimeError("String arguments expected");
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

DEF(nedf_get,
    "any get(object arrayOrMap, any keyOrIndex)",
    "misc",
    "Obtain a value from a NED map or array. "
    "Examples: get([10,20,30], 1) returns 20; get({foo:10,bar:20}, 'foo') returns 10. "
    "Note that get(x,i) may also be written as x.get(i), with the two being completely equivalent.")

cValue nedf_get(cComponent *contextComponent, cValue argv[], int argc)
{
    cObject *obj = argv[0].objectValue();
    cValue& ii = argv[1];
    if (cValueArray *arr = dynamic_cast<cValueArray *>(obj)) {
        long index = ii.intValue();
        return arr->get(index);
    }
    else if (cValueMap *map = dynamic_cast<cValueMap *>(obj)) {
        std::string key = ii.stringValue();
        return map->get(key.c_str());
    }
    else
        throw cRuntimeError("Unsupported C++ type '%s', get() only supports cValueArray and cValueMap", obj->getClassName());
}

DEF(nedf_size,
    "int size(object arrayOrMap)",
    "misc",
    "Returns the length of an array, or the number of elements in a map.")

cValue nedf_size(cComponent *contextComponent, cValue argv[], int argc)
{
    cObject *obj = argv[0].objectValue();
    if (cValueArray *arr = dynamic_cast<cValueArray *>(obj))
        return arr->size();
    else if (cValueMap *map = dynamic_cast<cValueMap *>(obj))
        return map->size();
    else
        throw cRuntimeError("Unsupported C++ type '%s', size() only supports cValueArray and cValueMap", obj->getClassName());
}

DEF(nedf_dup,
    "object dup(object obj)",
    "misc",
    "Clones the given object by calling its dup() C++ method.")

cValue nedf_dup(cComponent *contextComponent, cValue argv[], int argc)
{
    cObject *obj = argv[0].objectValue();
    return obj->dup();
}

}  // namespace omnetpp

