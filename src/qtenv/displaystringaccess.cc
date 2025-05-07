//==========================================================================
//   DISPLAYSTRINGACCESS.CC  -  part of
//                     OMNeT++/OMNEST
//            Discrete System Simulation in C++
//
//==========================================================================

/*--------------------------------------------------------------*
  Copyright (C) 1992-2017 Andras Varga
  Copyright (C) 2006-2017 OpenSim Ltd.

  This file is distributed WITHOUT ANY WARRANTY. See the file
  `license' for details on this and other legal matters.
*--------------------------------------------------------------*/

#include "displaystringaccess.h"

#include <cstring>
#include <cstdlib>
#include "omnetpp/cdisplaystring.h"
#include "omnetpp/cmodule.h" // due to getParentModule
#include "omnetpp/cdynamicexpression.h"
#include "common/opp_ctype.h"
#include "common/stringutil.h"

namespace omnetpp {

using namespace common;

namespace qtenv {

DisplayStringAccess::DisplayStringAccess(cComponent *component)
    : DisplayStringAccess(&component->getDisplayString(), component)
{

}

bool DisplayStringAccess::mayContainParamRefs(const char *arg)
{
    return strchr(arg, '$') != nullptr;
}

const char *DisplayStringAccess::getTagArg(const char *tagname, int index, std::string& buffer) const
{
    return substituteParamRefs(ds->getTagArg(tagname, index), buffer);
}

const char *DisplayStringAccess::getTagArg(int tagindex, int index, std::string& buffer) const
{
    return substituteParamRefs(ds->getTagArg(tagindex, index), buffer);
}

bool DisplayStringAccess::getTagArgAsBool(const char *tagname, int index, bool defaultValue, bool *okp) const
{
    return parseTagArgAsBool(ds->getTagArg(tagname, index), defaultValue, okp);
}

bool DisplayStringAccess::getTagArgAsBool(int tagindex, int index, bool defaultValue, bool *okp) const
{
    return parseTagArgAsBool(ds->getTagArg(tagindex, index), defaultValue, okp);
}

long DisplayStringAccess::getTagArgAsLong(const char *tagname, int index, long defaultValue, bool *okp) const
{
    return parseTagArgAsLong(ds->getTagArg(tagname, index), defaultValue, okp);
}

long DisplayStringAccess::getTagArgAsLong(int tagindex, int index, long defaultValue, bool *okp) const
{
    return parseTagArgAsLong(ds->getTagArg(tagindex, index), defaultValue, okp);
}

double DisplayStringAccess::getTagArgAsDouble(const char *tagname, int index, double defaultValue, bool *okp) const
{
    return parseTagArgAsDouble(ds->getTagArg(tagname, index), defaultValue, okp);
}

double DisplayStringAccess::getTagArgAsDouble(int tagindex, int index, double defaultValue, bool *okp) const
{
    return parseTagArgAsDouble(ds->getTagArg(tagindex, index), defaultValue, okp);
}

inline static void skip_whitespace(const char *&s)
{
    while (opp_isspace(*s))
        s++;
}

inline static void skip_whitespace(char *&s)
{
    while (opp_isspace(*s))
        s++;
}

inline static bool streq_trim(const char *literal, const char *s)
{
    skip_whitespace(s);
    int n = strlen(literal);

    if (strncmp(literal, s, n) == 0) {
        const char *endp = s + n;
        skip_whitespace(endp);
        return *endp == 0;
    }

    return false;
}

static bool strtob_ok(const char *s, bool defaultValue, bool *okp)
{
    if (streq_trim("false", s)) {
        if (okp)
            *okp = true;
        return false;
    }
    else if (streq_trim("true", s)) {
        if (okp)
            *okp = true;
        return true;
    }
    else {
        if (okp)
            *okp = false;
        return defaultValue;
    }
}

static long strtol_ok(const char *s, long defaultValue, bool *okp)
{
    char *endp;
    skip_whitespace(s);
    long result = strtol(s, &endp, 10);
    skip_whitespace(endp);

    bool success = (endp != s) && (*endp == 0);
    if (okp)
        *okp = success;
    return success ? result : defaultValue;
}

static double strtod_ok(const char *s, double defaultValue, bool *okp)
{
    char *endp;
    skip_whitespace(s);
    double result = strtod(s, &endp);
    skip_whitespace(endp);

    bool success = (endp != s) && (*endp == 0);
    if (okp)
        *okp = success;
    return success ? result : defaultValue;
}

bool DisplayStringAccess::parseTagArgAsBool(const char *arg, bool defaultValue, bool *okp) const
{
    if (!arg || !*arg) {
        if (okp)
            *okp = false;
        return defaultValue;
    }
    if (!mayContainParamRefs(arg))
        return strtob_ok(arg, defaultValue, okp);

    // This is only a shortcut, the externally visible behavior should
    // be equivalent to one using string substitution.
    cValue val = resolveSingleValue(arg);
    if (val.getType() == cValue::BOOL) {
        if (okp)
            *okp = true;
        return val.boolValue();
    }
    if (val.getType() == cValue::STRING)
        return strtob_ok(val.stringValue(), defaultValue, okp);

    std::string buffer;
    const char *arg2 = substituteParamRefs(arg, buffer);
    return strtob_ok(arg2, defaultValue, okp);
}

long DisplayStringAccess::parseTagArgAsLong(const char *arg, long defaultValue, bool *okp) const
{
    if (!arg || !*arg) {
        if (okp)
            *okp = false;
        return defaultValue;
    }
    if (!mayContainParamRefs(arg))
        return strtol_ok(arg, defaultValue, okp);

    // This is only a shortcut, the externally visible behavior should
    // be equivalent to one using string substitution.
    cValue val = resolveSingleValue(arg);
    if (val.getType() == cValue::INT) {
        if (okp)
            *okp = true;
        return val.intValue();
    }
    if (val.getType() == cValue::STRING)
        return strtol_ok(val.stringValue(), defaultValue, okp);

    std::string buffer;
    const char *arg2 = substituteParamRefs(arg, buffer);
    return strtol_ok(arg2, defaultValue, okp);
}

double DisplayStringAccess::parseTagArgAsDouble(const char *arg, double defaultValue, bool *okp) const
{
    if (!arg || !*arg) {
        if (okp)
            *okp = false;
        return defaultValue;
    }
    if (!mayContainParamRefs(arg))
        return strtod_ok(arg, defaultValue, okp);

    // This is only a shortcut, the externally visible behavior should
    // be equivalent to one using string substitution.
    cValue val = resolveSingleValue(arg);
    if (val.isNumeric()) {
        if (okp)
            *okp = true;
        return val.doubleValue();
    }
    if (val.getType() == cValue::STRING)
        return strtod_ok(val.stringValue(), defaultValue, okp);

    std::string buffer;
    const char *arg2 = substituteParamRefs(arg, buffer);
    return strtod_ok(arg2, defaultValue, okp);
}

cPar *DisplayStringAccess::lookupPar(const char *parname) const
{
    if (!component)
        throw cRuntimeError("cannot resolve parameter '%s': no context component", parname);

    // look up locally
    int k = component->findPar(parname);
    if (k >= 0)
        return &(component->par(k));

    // look up in parent
    if (component->getParentModule()) {
        k = component->getParentModule()->findPar(parname);
        if (k >= 0)
            return &(component->getParentModule()->par(k));
    }

    // not found -- generate suitable error message
    const char *what = component->isModule() ? "module" : "channel";
    throw cRuntimeError("%s '%s' and its parent have no parameter '%s'", what, component->getFullPath().c_str(), parname);
}

cValue DisplayStringAccess::resolveSingleValue(const char *arg) const
{
    if (arg[0] != '$')
        return cValue();
    if (arg[1] == '{') {
        int len = opp_strlen(arg);
        const char *end = arg + (len-1); // points to the last character, not the terminating null
        if (*end != '}' || opp_findmatchingparen(arg+1) != end)
            return cValue();

        std::string exprText = std::string(arg+2, len-3); // skip ${ and }
        cDynamicExpression expr;
        expr.parseNedExpr(exprText.c_str());
        return expr.evaluate(component);
    }
    else {
        cPar *par = lookupPar(arg+1);  // rest of string is assumed to be the param name
        return par->getValue();
    }
}

const char *DisplayStringAccess::substituteParamRefs(const char *src, std::string& buffer) const
{
    if (!mayContainParamRefs(src))
        return src;

    // recognize "$param" and "${param}" syntax inside the string
    buffer = "";
    for (const char *s = src; *s; ) {
        if (*s != '$')
            buffer += *s++;
        else {
            s++;  // skip '$'
            if (opp_isalphaext(*s)) {
                // $parname
                std::string name;
                while (opp_isalnumext(*s))
                    name += *s++;
                // append its value
                cPar *par = lookupPar(name.c_str());
                cValue value = par->getValue();
                buffer += value.getType() == cValue::STRING ? value.stdstringValue() : value.str();
            }
            else if (*s == '{') {
                // ${ned-expression}
                const char *end = opp_findmatchingparen(s);
                if (end == nullptr)
                    throw cRuntimeError("Unclosed '{' in display string");

                std::string exprText = std::string(s + 1, end - s - 1); // skip { and }
                cDynamicExpression expr;
                expr.parseNedExpr(exprText.c_str());
                cValue value = expr.evaluate(component);
                buffer += value.getType() == cValue::STRING ? value.stdstringValue() : value.str();
                s = end + 1;
            }
            else if (*s == '$') {
                buffer += *s++;
            }
            else {
                throw cRuntimeError("Illegal '$' in display string, $parName, ${NED-expression}, or $$ expected");
            }
        }
    }

    return buffer.c_str();
}

} // namespace qtenv
} // namespace omnetpp