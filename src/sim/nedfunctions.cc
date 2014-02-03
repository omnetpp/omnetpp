//=========================================================================
//  NEDFUNCTIONS.CC - part of
//
//                    OMNeT++/OMNEST
//             Discrete System Simulation in C++
//
//=========================================================================

/*--------------------------------------------------------------*
  Copyright (C) 1992-2008 Andras Varga
  Copyright (C) 2006-2008 OpenSim Ltd.

  This file is distributed WITHOUT ANY WARRANTY. See the file
  `license' for details on this and other legal matters.
*--------------------------------------------------------------*/

#include <math.h>
#include <string.h>
#include <algorithm>
#include "distrib.h"
#include "cnedmathfunction.h"
#include "cnedfunction.h"
#include "cexception.h"
#include "cstringtokenizer.h"
#include "stringutil.h"
#include "opp_ctype.h"
#include "cconfiguration.h"
#include "ccomponenttype.h"

NAMESPACE_BEGIN

//FIXME cDynamicExpression to add function name to exceptions thrown from functions

void nedfunctions_dummy() {} //see util.cc

#define DEF(FUNCTION, SIGNATURE, CATEGORY, DESCRIPTION, BODY) \
    static cNEDValue FUNCTION(cComponent *context, cNEDValue argv[], int argc) {BODY} \
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


DEF(nedf_fabs,
    "quantity fabs(quantity x)",
    "math",
    "Returns the absolute value of the quantity.",
{
    return cNEDValue(fabs((double)argv[0]), argv[0].getUnit());
})

DEF(nedf_fmod,
    "quantity fmod(quantity x, quantity y)",
    "math",
    "Returns the floating-point remainder of x/y; unit conversion takes place if needed.",
{
    double argv1converted = argv[1].doubleValueInUnit(argv[0].getUnit());
    return cNEDValue(fmod((double)argv[0], argv1converted), argv[0].getUnit());
})

DEF(nedf_min,
    "quantity min(quantity a, quantity b)",
    "math",
    "Returns the smaller one of the two quantities; unit conversion takes place if needed.",
{
    double argv1converted = argv[1].doubleValueInUnit(argv[0].getUnit());
    return (double)argv[0] < argv1converted ? argv[0] : argv[1];
})

DEF(nedf_max,
    "quantity max(quantity a, quantity b)",
    "math",
    "Returns the greater one of the two quantities; unit conversion takes place if needed.",
{
    double argv1converted = argv[1].doubleValueInUnit(argv[0].getUnit());
    return (double)argv[0] < argv1converted ? argv[1] : argv[0];
})


//
// Unit handling
//

DEF(nedf_dropUnit,
    "double dropUnit(quantity x)",
    "units",
    "Removes the unit of measurement from quantity x.",
{
    argv[0].setUnit(NULL);
    return argv[0];
})

DEF(nedf_replaceUnit,
    "quantity replaceUnit(quantity x, string unit)",
    "units",
    "Replaces the unit of x with the given unit.",
{
    const char *newUnit = cNEDValue::getPooled((const char *)argv[1]);
    argv[0].setUnit(newUnit);
    return argv[0];
})

DEF(nedf_convertUnit,
    "quantity convertUnit(quantity x, string unit)",
    "units",
    "Converts x to the given unit.",
{
    const char *newUnit = cNEDValue::getPooled((const char *)argv[1]);
    argv[0].convertTo(newUnit);
    return argv[0];
})

DEF(nedf_unitOf,
    "string unitOf(quantity x)",
    "units",
    "Returns the unit of the given quantity.",
{
    return argv[0].getUnit();
})


//
// String manipulation functions.
//

DEF(nedf_length,
    "int length(string s)",
    "strings",
    "Returns the length of the string",
{
    return (long)argv[0].stdstringValue().size();
})

DEF(nedf_contains,
    "bool contains(string s, string substr)",
    "strings",
    "Returns true if string s contains substr as substring",
{
    return argv[0].stdstringValue().find(argv[1].stdstringValue()) != std::string::npos;
})

DEF(nedf_substring,
    "string substring(string s, int pos, int len?)",
    "strings",
    "Return the substring of s starting at the given position, either to the end of the string or maximum len characters",
{
    int size = argv[0].stdstringValue().size();
    int index = (int)argv[1];
    int length = argc==3 ? (int)argv[2] : size-index;

    if (index < 0 || index > size)
        throw cRuntimeError("substring(): index out of bounds");
    if (length < 0)
        throw cRuntimeError("substring(): length is negative");
    return argv[0].stdstringValue().substr(index, length);
})

DEF(nedf_substringBefore,
    "string substringBefore(string s, string substr)",
    "strings",
    "Returns the substring of s before the first occurrence of substr, or the empty string if s does not contain substr.",
{
    size_t pos = argv[0].stdstringValue().find(argv[1].stdstringValue());
    return pos==std::string::npos ? "" : argv[0].stdstringValue().substr(0,pos);
})

DEF(nedf_substringAfter,
    "string substringAfter(string s, string substr)",
    "strings",
    "Returns the substring of s after the first occurrence of substr, or the empty string if s does not contain substr.",
{
    size_t pos = argv[0].stdstringValue().find(argv[1].stdstringValue());
    return pos==std::string::npos ? "" : argv[0].stdstringValue().substr(pos+argv[1].stdstringValue().size());
})

DEF(nedf_substringBeforeLast,
    "string substringBeforeLast(string s, string substr)",
    "strings",
    "Returns the substring of s before the last occurrence of substr, or the empty string if s does not contain substr.",
{
    size_t pos = argv[0].stdstringValue().rfind(argv[1].stdstringValue());
    return pos==std::string::npos ? "" : argv[0].stdstringValue().substr(0,pos);
})

DEF(nedf_substringAfterLast,
    "string substringAfterLast(string s, string substr)",
    "strings",
    "Returns the substring of s after the last occurrence of substr, or the empty string if s does not contain substr.",
{
    size_t pos = argv[0].stdstringValue().rfind(argv[1].stdstringValue());
    return pos==std::string::npos ? "" : argv[0].stdstringValue().substr(pos+argv[1].stdstringValue().size());
})

DEF(nedf_startsWith,
    "bool startsWith(string s, string substr)",
    "strings",
    "Returns true if s begins with the substring substr.",
{
    return argv[0].stdstringValue().find(argv[1].stdstringValue()) == 0;
})

DEF(nedf_endsWith,
    "bool endsWith(string s, string substr)",
    "strings",
    "Returns true if s ends with the substring substr.",
{
    return argv[0].stdstringValue().rfind(argv[1].stdstringValue()) == argv[0].stdstringValue().size() - argv[1].stdstringValue().size();
})

DEF(nedf_tail,
    "string tail(string s, int len)",
    "strings",
    "Returns the last len character of s, or the full s if it is shorter than len characters.",
{
    int length = (int)argv[1];
    if (length < 0)
        throw cRuntimeError("tail(): length is negative");
    int size = (int) argv[0].stdstringValue().size();
    return argv[0].stdstringValue().substr(std::max(0, size - length), size);
})

DEF(nedf_replace,
    "string replace(string s, string substr, string repl, int startPos?)",
    "strings",
    "Replaces all occurrences of substr in s with the string repl. If startPos is given, search begins from position startPos in s.",
{
    std::string str = argv[0].stdstringValue();
    const std::string& search = argv[1].stdstringValue();
    const std::string& replacement = argv[2].stdstringValue();
    int index = 0;
    if (argc==4) {
        index = (int)argv[3];
        if (index < 0)
            throw cRuntimeError("replace(): start index is negative");
        if (index > (int)str.size())
            throw cRuntimeError("replace(): start index out of bounds");
    }

    size_t searchSize = search.size();
    size_t replacementSize = replacement.size();
    while ((size_t)(index = str.find(search, index)) != std::string::npos) {
        str.replace(index, searchSize, replacement);
        index += replacementSize - searchSize + 1;
    }
    return str;
})

DEF(nedf_replaceFirst,
    "string replaceFirst(string s, string substr, string repl, int startPos?)",
    "strings",
    "Replaces the first occurrence of substr in s with the string repl. If startPos is given, search begins from position startPos in s.",
{
    std::string str = argv[0].stdstringValue();
    const std::string& search = argv[1].stdstringValue();
    const std::string& replacement = argv[2].stdstringValue();
    int index = 0;
    if (argc==4) {
        index = (int)argv[3];
        if (index < 0)
            throw cRuntimeError("replaceFirst(): start index is negative");
        if (index > (int)str.size())
            throw cRuntimeError("replaceFirst(): start index out of bounds");
    }

    size_t searchSize = search.size();
    if ((size_t)(index = str.find(search, index)) != std::string::npos)
        str.replace(index, searchSize, replacement);
    return str;
})

DEF(nedf_trim,
    "string trim(string s)",
    "strings",
    "Discards whitespace from the start and end of s, and returns the result.",
{
    return opp_trim(argv[0].stdstringValue().c_str());
})

DEF(nedf_indexOf,
    "int indexOf(string s, string substr)",
    "strings",
    "Returns the position of the first occurrence of substring substr in s, or -1 if s does not contain substr.",
{
    return (long)argv[0].stdstringValue().find(argv[1].stdstringValue());
})

DEF(nedf_choose,
    "string choose(int index, string list)",
    "strings",
    "Interprets list as a space-separated list, and returns the item at the given index. Negative and out-of-bounds indices cause an error.",
{
    int index = (int)argv[0];
    if (index < 0)
        throw cRuntimeError("choose(): negative index");
    cStringTokenizer tokenizer(argv[1].stdstringValue().c_str());
    for (int i=0; i<index && tokenizer.hasMoreTokens(); i++)
        tokenizer.nextToken();
    if (!tokenizer.hasMoreTokens())
        throw cRuntimeError("choose(): index out of bounds: %d", index);
    return tokenizer.nextToken();
})

DEF(nedf_toUpper,
    "string toUpper(string s)",
    "strings",
    "Converts s to all uppercase, and returns the result.",
{
    std::string tmp = argv[0].stdstringValue();
    int length = tmp.length();
    for (int i=0; i<length; i++)
        tmp[i] = opp_toupper(tmp[i]);
    return tmp;
})

DEF(nedf_toLower,
    "string toLower(string s)",
    "strings",
    "Converts s to all lowercase, and returns the result.",
{
    std::string tmp = argv[0].stdstringValue();
    int length = tmp.length();
    for (int i=0; i<length; i++)
        tmp[i] = opp_tolower(tmp[i]);
    return tmp;
})

DEF(nedf_expand,
    "string expand(string s)",
    "strings",
    "Expands ${} variables (${configname}, ${runnumber}, etc.) in the given string, and returns the result.",
{
    std::string tmp = argv[0].stdstringValue();
    tmp = ev.getConfig()->substituteVariables(tmp.c_str());
    return tmp;
})

DEF(nedf_int,
    "int int(any x)",
    "conversion",
    "Converts x to an integer (C++ long), and returns the result. A boolean argument becomes 0 or 1; a double is converted using floor(); a string is interpreted as number; an XML argument causes an error.",
{
    switch (argv[0].getType()) {
        case cNEDValue::BOOL:
            return (bool)argv[0] ? 1L : 0L;
        case cNEDValue::DBL:
            return (long)floor((double)argv[0]);
        case cNEDValue::STR:
            return (long)floor(opp_atof(argv[0].stringValue()));  //XXX catch & wrap exception?
        case cNEDValue::XML:
            throw cRuntimeError("int(): cannot convert xml to int");
        default:
            throw cRuntimeError("internal error: bad cNEDValue type");
    }
})

DEF(nedf_double,
    "double double(any x)",
    "conversion",
    "Converts x to double, and returns the result. A boolean argument becomes 0 or 1; a string is interpreted as number; an XML argument causes an error.",
{
    switch (argv[0].getType()) {
        case cNEDValue::BOOL:
            return (bool)argv[0] ? 1.0 : 0.0;
        case cNEDValue::DBL:
            return (double)argv[0];
        case cNEDValue::STR:
            return opp_atof(argv[0].stringValue());  //XXX catch & wrap exception?
        case cNEDValue::XML:
            throw cRuntimeError("double(): cannot convert xml to double");
        default:
            throw cRuntimeError("internal error: bad cNEDValue type");
    }
})

DEF(nedf_string,
    "string string(any x)",
    "conversion",
    "Converts x to string, and returns the result.",
{
    return argv[0].str();
})


//
// Reflection
//

DEF(nedf_fullPath,
    "string fullPath()",
    "ned",
    "Returns the full path of the module or channel in context.",
{
    return context->getFullPath();
})

DEF(nedf_fullName,
    "string fullName()",
    "ned",
    "Returns the full name of the module or channel in context.",
{
    return context->getFullName();
})

DEF(nedf_parentIndex,
    "int parentIndex()",
    "ned",
    "Returns the index of the parent module, which has to be part of module vector.",
{
    cModule *mod = context->getParentModule();
    if (!mod)
        throw cRuntimeError("parentIndex(): `%s' has no parent module", context->getFullPath().c_str());
    if (!mod->isVector())
        throw cRuntimeError("parentIndex(): module `%s' is not a vector", mod->getFullPath().c_str());
    return (long)mod->getIndex();
})

DEF(nedf_ancestorIndex,
    "int ancestorIndex(int numLevels)",
    "ned",
    "Returns the index of the ancestor module numLevels levels above the module or channel in context.",
{
    int levels = (int)argv[0];
    if (levels<0)
        throw cRuntimeError("ancestorIndex(): negative number of levels");
    if (levels==0 && !context->isModule())
        throw cRuntimeError("ancestorIndex(): numlevels==0 and this is not a module");
    cModule *mod = dynamic_cast<cModule *>(context);
    for (int i=0; mod && i<levels; i++)
        mod = mod->getParentModule();
    if (!mod)
        throw cRuntimeError("ancestorIndex(): argument is larger than current nesting level");
    if (!mod->isVector())
        throw cRuntimeError("ancestorIndex(): module `%s' is not a vector", mod->getFullPath().c_str());
    return (long) mod->getIndex();
})


//
// Random variate generation.
//

// continuous
DEF(nedf_uniform,
    "quantity uniform(quantity a, quantity b, int rng?)",
    "random/continuous",
    "Returns a random number from the Uniform distribution",
{
    int rng = argc==3 ? (int)argv[2] : 0;
    double argv1converted = argv[1].doubleValueInUnit(argv[0].getUnit());
    return cNEDValue(uniform((double)argv[0], argv1converted, rng), argv[0].getUnit());
})

DEF(nedf_exponential,
    "quantity exponential(quantity mean, int rng?)",
    "random/continuous",
    "Returns a random number from the Exponential distribution",
{
    int rng = argc==2 ? (int)argv[1] : 0;
    return cNEDValue(exponential((double)argv[0], rng), argv[0].getUnit());
})

DEF(nedf_normal,
    "quantity normal(quantity mean, quantity stddev, int rng?)",
    "random/continuous",
    "Returns a random number from the Normal distribution",
{
    int rng = argc==3 ? (int)argv[2] : 0;
    double argv1converted = argv[1].doubleValueInUnit(argv[0].getUnit());
    return cNEDValue(normal((double)argv[0], argv1converted, rng), argv[0].getUnit());
})

DEF(nedf_truncnormal,
    "quantity truncnormal(quantity mean, quantity stddev, int rng?)",
    "random/continuous",
    "Returns a random number from the truncated Normal distribution",
{
    int rng = argc==3 ? (int)argv[2] : 0;
    double argv1converted = argv[1].doubleValueInUnit(argv[0].getUnit());
    return cNEDValue(truncnormal((double)argv[0], argv1converted, rng), argv[0].getUnit());
})

DEF(nedf_gamma_d,
    "quantity gamma_d(double alpha, quantity theta, int rng?)",
    "random/continuous",
    "Returns a random number from the Gamma distribution",
{
    int rng = argc==3 ? (int)argv[2] : 0;
    return cNEDValue(gamma_d((double)argv[0], (double)argv[1], rng), argv[1].getUnit());
})

DEF(nedf_beta,
    "double beta(double alpha1, double alpha2, int rng?)",
    "random/continuous",
    "Returns a random number from the Beta distribution",
{
    int rng = argc==3 ? (int)argv[2] : 0;
    argv[0] = beta((double)argv[0], (double)argv[1], rng);
    return argv[0];
})

DEF(nedf_erlang_k,
    "quantity erlang_k(int k, quantity mean, int rng?)",
    "random/continuous",
    "Returns a random number from the Erlang distribution",
{
    int k = (int)argv[0];
    if (k < 0)
        throw cRuntimeError("erlang_k(): k parameter (number of phases) must be positive (k=%d)", k);
    int rng = argc==3 ? (int)argv[2] : 0;
    return cNEDValue(erlang_k(k, (double)argv[1], rng), argv[1].getUnit());
})

DEF(nedf_chi_square,
    "double chi_square(int k, int rng?)",
    "random/continuous",
    "Returns a random number from the Chi-square distribution",
{
    int k = (int)argv[0];
    if (k < 0)
        throw cRuntimeError("chi_square(): k parameter (degrees of freedom) must be positive (k=%d)", k);
    int rng = argc==2 ? (int)argv[1] : 0;
    return chi_square(k, rng);
})

DEF(nedf_student_t,
    "double student_t(int i, int rng?)",
    "random/continuous",
    "Returns a random number from the Student-t distribution",
{
    int i = (int)argv[0];
    if (i < 0)
       throw cRuntimeError("student_t(): i parameter (degrees of freedom) must be positive (i=%d)", i);
    int rng = argc==2 ? (int)argv[1] : 0;
    return student_t(i, rng);
})

DEF(nedf_cauchy,
    "quantity cauchy(quantity a, quantity b, int rng?)",
    "random/continuous",
    "Returns a random number from the Cauchy distribution",
{
    int rng = argc==3 ? (int)argv[2] : 0;
    double argv1converted = argv[1].doubleValueInUnit(argv[0].getUnit());
    return cNEDValue(cauchy((double)argv[0], argv1converted, rng), argv[0].getUnit());
})

DEF(nedf_triang,
    "quantity triang(quantity a, quantity b, quantity c, int rng?)",
    "random/continuous",
    "Returns a random number from the Triangular distribution",
{
    int rng = argc==4 ? (int)argv[3] : 0;
    double argv1converted = argv[1].doubleValueInUnit(argv[0].getUnit());
    double argv2converted = argv[2].doubleValueInUnit(argv[0].getUnit());
    return cNEDValue(triang((double)argv[0], argv1converted, argv2converted, rng), argv[0].getUnit());
})

DEF(nedf_lognormal,
    "double lognormal(double m, double w, int rng?)",
    "random/continuous",
    "Returns a random number from the Lognormal distribution",
{
    int rng = argc==3 ? (int)argv[2] : 0;
    return lognormal((double)argv[0], (double)argv[1], rng);
})

DEF(nedf_weibull,
    "quantity weibull(quantity a, quantity b, int rng?)",
    "random/continuous",
    "Returns a random number from the Weibull distribution",
{
    int rng = argc==3 ? (int)argv[2] : 0;
    double argv1converted = argv[1].doubleValueInUnit(argv[0].getUnit());
    return cNEDValue(weibull((double)argv[0], argv1converted, rng), argv[0].getUnit());
})

DEF(nedf_pareto_shifted,
    "quantity pareto_shifted(double a, quantity b, quantity c, int rng?)",
    "random/continuous",
    "Returns a random number from the Pareto-shifted distribution",
{
    int rng = argc==4 ? (int)argv[3] : 0;
    double argv2converted = argv[2].doubleValueInUnit(argv[1].getUnit());
    return cNEDValue(pareto_shifted((double)argv[0], (double)argv[1], argv2converted, rng), argv[1].getUnit());
})

// discrete

DEF(nedf_intuniform,
    "int intuniform(int a, int b, int rng?)",
    "random/discrete",
    "Returns a random number from the Intuniform distribution",
{
    int rng = argc==3 ? (int)argv[2] : 0;
    return (long) intuniform((int)argv[0], (int)argv[1], rng);
})

DEF(nedf_bernoulli,
    "int bernoulli(double p, int rng?)",
    "random/discrete",
    "Returns a random number from the Bernoulli distribution",
{
    int rng = argc==2 ? (int)argv[1] : 0;
    return (long) bernoulli((double)argv[0], rng);
})

DEF(nedf_binomial,
    "int binomial(int n, double p, int rng?)",
    "random/discrete",
    "Returns a random number from the Binomial distribution",
{
    int rng = argc==3 ? (int)argv[2] : 0;
    return (long) binomial((int)argv[0], (double)argv[1], rng);
})

DEF(nedf_geometric,
    "int geometric(double p, int rng?)",
    "random/discrete",
    "Returns a random number from the Geometric distribution",
{
    int rng = argc==2 ? (int)argv[1] : 0;
    return (long) geometric((double)argv[0], rng);
})

DEF(nedf_negbinomial,
    "int negbinomial(int n, double p, int rng?)",
    "random/discrete",
    "Returns a random number from the Negbinomial distribution",
{
    int rng = argc==3 ? (int)argv[2] : 0;
    return (long) negbinomial((int)argv[0], (double)argv[1], rng);
})

DEF(nedf_poisson,
    "int poisson(double lambda, int rng?)",
    "random/discrete",
    "Returns a random number from the Poisson distribution",
{
    int rng = argc==2 ? (int)argv[1] : 0;
    return (long) poisson((double)argv[0], rng);
})

//
// XML functions
//

DEF(nedf_xmldoc,
    "xml xmldoc(string filename, string xpath?)",
    "xml",
    "Parses the given XML file into a cXMLElement tree, and returns the root element. "
    "When called with two arguments, it returns the first element from the tree that "
    "matches the expression given in simplified XPath syntax.",
{
    const char *filename = argv[0].stringValue();
    const char *xpath = argc==1 ? NULL : argv[1].stringValue();
    cXMLElement *node = ev.getXMLDocument(filename, xpath);
    if (!node)
    {
        if (argc==1)
            throw cRuntimeError("xmldoc(\"%s\"): element not found", filename);
        else
            throw cRuntimeError("xmldoc(\"%s\", \"%s\"): element not found", filename, xpath);
    }
    return node;
})

DEF(nedf_xml,
    "xml xml(string xmlstring, string xpath?)",
    "xml",
    "Parses the given XML string into a cXMLElement tree, and returns the root element. "
    "When called with two arguments, it returns the first element from the tree that "
    "matches the expression given in simplified XPath syntax.",
{
    cXMLElement *node;
    if (argc==1)
    {
        node = ev.getParsedXMLString(argv[0].stdstringValue().c_str(), NULL);
        if (!node)
            throw cRuntimeError("xml(\"%s\"): element not found", argv[0].stdstringValue().c_str());
    }
    else
    {
        node = ev.getParsedXMLString(argv[0].stdstringValue().c_str(), argv[1].stdstringValue().c_str());
        if (!node)
            throw cRuntimeError("xml(\"%s\", \"%s\"): element not found", argv[0].stdstringValue().c_str(), argv[1].stdstringValue().c_str());
    }
    return node;
})


//
// misc utility functions
//

DEF(nedf_simTime,
    "quantity simTime()",
    "misc",
    "Returns the current simulation time.",
{
    return cNEDValue(SIMTIME_DBL(simTime()), "s");
})

DEF(nedf_select,
    "any select(int index, ...)",
    "misc",
    "Returns the <index>th item from the rest of the argument list; numbering starts from 0.",
{
    long index = argv[0];
    if (index < 0)
        throw cRuntimeError("select(): negative index %ld", index);
    if (index >= argc-1)
        throw cRuntimeError("select(): index=%ld is too large", index, argc-1);
    return argv[index+1];
})

DEF(nedf_firstAvailable,
    "string firstAvailable(...)",
    "misc",
    "Accepts any number of strings, interprets them as NED type names "
    "(qualified or unqualified), and returns the first one that exists and "
    "its C++ implementation class is also available. Throws an error if "
    "none of the types are available.",
{
    cRegistrationList *types = componentTypes.getInstance();
    for (int i=0; i<argc; i++)
    {
        if (argv[i].getType() != cNEDValue::STR)
            throw cRuntimeError("firstAvailable(): string arguments expected");
        const char *name = argv[i].stringValue();
        cComponentType *c;
        c = dynamic_cast<cComponentType *>(types->lookup(name)); // by qualified name
        if (c && c->isAvailable())
            return argv[i];
        c = dynamic_cast<cComponentType *>(types->find(name)); // by simple name
        if (c && c->isAvailable())
            return argv[i];
    }

    std::string typelist;
    for (int i=0; i<argc; i++)
        typelist += std::string(i==0?"":", ") + argv[i].stdstringValue();
    throw cRuntimeError("None of the following NED types are available: %s", typelist.c_str());
})

NAMESPACE_END

