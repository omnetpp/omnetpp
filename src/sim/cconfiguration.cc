//=========================================================================
//  CCONFIGURATION.CC - part of
//
//                  OMNeT++/OMNEST
//           Discrete System Simulation in C++
//
//  Author: Andras Varga
//
//=========================================================================

/*--------------------------------------------------------------*
  Copyright (C) 2003-2008 Andras Varga
  Copyright (C) 2006-2008 OpenSim Ltd.

  This file is distributed WITHOUT ANY WARRANTY. See the file
  `license' for details on this and other legal matters.
*--------------------------------------------------------------*/


#include <errno.h>
#include "cconfiguration.h"
#include "cconfigoption.h"
#include "unitconversion.h"
#include "stringutil.h"
#include "fileutil.h"
#include "stringtokenizer.h"
#include "fnamelisttokenizer.h"
#include "exception.h"

NAMESPACE_BEGIN


bool cConfiguration::parseBool(const char *s, const char *defaultValue, bool fallbackValue)
{
    if (!s)
        s = defaultValue;
    if (!s)
        return fallbackValue;
    if (strcmp(s,"yes")==0 || strcmp(s,"true")==0)
        return true;
    else if (strcmp(s,"no")==0 || strcmp(s,"false")==0)
        return false;
    else
        throw opp_runtime_error("`%s' is not a valid boolean value, use true/false", s);
}

long cConfiguration::parseLong(const char *s, const char *defaultValue, long fallbackValue)
{
    if (!s)
        s = defaultValue;
    if (!s)
        return fallbackValue;
    return opp_atol(s);
}

double cConfiguration::parseDouble(const char *s, const char *unit, const char *defaultValue, double fallbackValue)
{
    if (!s)
        s = defaultValue;
    if (!s)
        return fallbackValue;
    return UnitConversion::parseQuantity(s, unit);
}

std::string cConfiguration::parseString(const char *s, const char *defaultValue, const char *fallbackValue)
{
    if (!s)
        s = defaultValue;
    if (!s)
        return fallbackValue;
    if (*s == '"')
        return opp_parsequotedstr(s);
    else
        return s;
}

std::string cConfiguration::parseFilename(const char *s, const char *baseDir, const char *defaultValue)
{
    if (!s)
        s = defaultValue;
    if (!s || !s[0])
        return "";
    return tidyFilename(concatDirAndFile(baseDir, s).c_str());
}

std::vector<std::string> cConfiguration::parseFilenames(const char *s, const char *baseDir, const char *defaultValue)
{
    if (!s)
        s = defaultValue;
    if (!s)
        s = "";
    std::vector<std::string> result;
    FilenamesListTokenizer tokenizer(s);
    const char *fname;
    while ((fname = tokenizer.nextToken())!=NULL)
    {
        if (fname[0]=='@' && fname[1]=='@')
            result.push_back("@@" + tidyFilename(concatDirAndFile(baseDir, fname+2).c_str()));
        else if (fname[0]=='@')
            result.push_back("@" + tidyFilename(concatDirAndFile(baseDir, fname+1).c_str()));
        else
            result.push_back(tidyFilename(concatDirAndFile(baseDir, fname).c_str()));
    }
    return result;
}

std::string cConfiguration::adjustPath(const char *s, const char *baseDir, const char *defaultValue)
{
    if (!s)
        s = defaultValue;
    if (!s)
        s = "";
    std::string result;
    StringTokenizer tokenizer(s, PATH_SEPARATOR);
    const char *dirName;
    while ((dirName = tokenizer.nextToken())!=NULL)
    {
        if (result.size()!=0)
            result += ";";
        result += tidyFilename(concatDirAndFile(baseDir, dirName).c_str());
    }
    return result;
}

//---

#define TRY(CODE)   try { CODE; } catch (std::exception& e) {throw cRuntimeError("Error getting option %s= from the configuration: %s", option->getName(), e.what());}

inline const char *fallback(const char *s, const char *defaultValue, const char *fallbackValue)
{
    return s!=NULL ? s : defaultValue!=NULL ? defaultValue : fallbackValue;
}

static void assertType(cConfigOption *option, bool isPerObject, cConfigOption::Type requiredType)
{
    if (option->isPerObject() != isPerObject)
    {
        if (option->isPerObject())
            throw cRuntimeError("Option %s= is read from the configuration in the wrong way: it is per-object configuration", option->getName());
        else
            throw cRuntimeError("Option %s= is read from the configuration in the wrong way: it is global (not per-object) configuration", option->getName());
    }
    if (option->getType() != requiredType)
        throw cRuntimeError("Option %s= is read from the configuration with the wrong type (type=%s, actual=%s)",
                            option->getName(), cConfigOption::getTypeName(requiredType), cConfigOption::getTypeName(option->getType()));
}

const char *cConfiguration::getAsCustom(cConfigOption *option, const char *fallbackValue)
{
    assertType(option, false, cConfigOption::CFG_CUSTOM);
    TRY(return fallback(getConfigValue(option->getName()), substituteVariables(option->getDefaultValue()), fallbackValue));
}

bool cConfiguration::getAsBool(cConfigOption *option, bool fallbackValue)
{
    assertType(option, false, cConfigOption::CFG_BOOL);
    TRY(return parseBool(getConfigValue(option->getName()), substituteVariables(option->getDefaultValue()), fallbackValue));
}

long cConfiguration::getAsInt(cConfigOption *option, long fallbackValue)
{
    assertType(option, false, cConfigOption::CFG_INT);
    TRY(return parseLong(getConfigValue(option->getName()), substituteVariables(option->getDefaultValue()), fallbackValue));
}

double cConfiguration::getAsDouble(cConfigOption *option, double fallbackValue)
{
    assertType(option, false, cConfigOption::CFG_DOUBLE);
    TRY(return parseDouble(getConfigValue(option->getName()), option->getUnit(), substituteVariables(option->getDefaultValue()), fallbackValue));
}

std::string cConfiguration::getAsString(cConfigOption *option, const char *fallbackValue)
{
    assertType(option, false, cConfigOption::CFG_STRING);
    TRY(return parseString(getConfigValue(option->getName()), substituteVariables(option->getDefaultValue()), fallbackValue));
}

std::string cConfiguration::getAsFilename(cConfigOption *option)
{
    assertType(option, false, cConfigOption::CFG_FILENAME);
    const KeyValue& keyvalue = getConfigEntry(option->getName());
    TRY(return parseFilename(keyvalue.getValue(), keyvalue.getBaseDirectory(), substituteVariables(option->getDefaultValue())));
}

std::vector<std::string> cConfiguration::getAsFilenames(cConfigOption *option)
{
    assertType(option, false, cConfigOption::CFG_FILENAMES);
    const KeyValue& keyvalue = getConfigEntry(option->getName());
    TRY(return parseFilenames(keyvalue.getValue(), keyvalue.getBaseDirectory(), substituteVariables(option->getDefaultValue())));
}

std::string cConfiguration::getAsPath(cConfigOption *option)
{
    assertType(option, false, cConfigOption::CFG_PATH);
    const KeyValue& keyvalue = getConfigEntry(option->getName());
    TRY(return adjustPath(keyvalue.getValue(), keyvalue.getBaseDirectory(), substituteVariables(option->getDefaultValue())));
}

//----

const char *cConfiguration::getAsCustom(const char *objectFullPath, cConfigOption *option, const char *fallbackValue)
{
    assertType(option, true, cConfigOption::CFG_CUSTOM);
    TRY(return fallback(getPerObjectConfigValue(objectFullPath, option->getName()), substituteVariables(option->getDefaultValue()), fallbackValue));
}

bool cConfiguration::getAsBool(const char *objectFullPath, cConfigOption *option, bool fallbackValue)
{
    assertType(option, true, cConfigOption::CFG_BOOL);
    TRY(return parseBool(getPerObjectConfigValue(objectFullPath, option->getName()), substituteVariables(option->getDefaultValue()), fallbackValue));
}

long cConfiguration::getAsInt(const char *objectFullPath, cConfigOption *option, long fallbackValue)
{
    assertType(option, true, cConfigOption::CFG_INT);
    TRY(return parseLong(getPerObjectConfigValue(objectFullPath, option->getName()), substituteVariables(option->getDefaultValue()), fallbackValue));
}

double cConfiguration::getAsDouble(const char *objectFullPath, cConfigOption *option, double fallbackValue)
{
    assertType(option, true, cConfigOption::CFG_DOUBLE);
    TRY(return parseDouble(getPerObjectConfigValue(objectFullPath, option->getName()), option->getUnit(), substituteVariables(option->getDefaultValue()), fallbackValue));
}

std::string cConfiguration::getAsString(const char *objectFullPath, cConfigOption *option, const char *fallbackValue)
{
    assertType(option, true, cConfigOption::CFG_STRING);
    TRY(return parseString(getPerObjectConfigValue(objectFullPath, option->getName()), substituteVariables(option->getDefaultValue()), fallbackValue));
}

std::string cConfiguration::getAsFilename(const char *objectFullPath, cConfigOption *option)
{
    assertType(option, true, cConfigOption::CFG_FILENAME);
    const KeyValue& keyvalue = getConfigEntry(option->getName());
    TRY(return parseFilename(keyvalue.getValue(), keyvalue.getBaseDirectory(), substituteVariables(option->getDefaultValue())));
}

std::vector<std::string> cConfiguration::getAsFilenames(const char *objectFullPath, cConfigOption *option)
{
    assertType(option, true, cConfigOption::CFG_FILENAMES);
    const KeyValue& keyvalue = getConfigEntry(option->getName());
    TRY(return parseFilenames(keyvalue.getValue(), keyvalue.getBaseDirectory(), substituteVariables(option->getDefaultValue())));
}

std::string cConfiguration::getAsPath(const char *objectFullPath, cConfigOption *option)
{
    assertType(option, true, cConfigOption::CFG_FILENAMES);
    const KeyValue& keyvalue = getConfigEntry(option->getName());
    TRY(return adjustPath(keyvalue.getValue(), keyvalue.getBaseDirectory(), substituteVariables(option->getDefaultValue())));
}

NAMESPACE_END

