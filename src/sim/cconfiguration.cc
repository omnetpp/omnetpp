//=========================================================================
//  CCONFIG.CC - part of
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

USING_NAMESPACE


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

#define TRY(CODE)   try { CODE; } catch (std::exception& e) {throw cRuntimeError("Error getting entry %s= from the configuration: %s", entry->getName(), e.what());}

inline const char *fallback(const char *s, const char *defaultValue, const char *fallbackValue)
{
    return s!=NULL ? s : defaultValue!=NULL ? defaultValue : fallbackValue;
}

static void assertType(cConfigOption *entry, bool isPerObject, cConfigOption::Type requiredType)
{
    if (entry->isPerObject() != isPerObject)
    {
        if (entry->isPerObject())
            throw cRuntimeError("Entry %s= is read from the configuration in the wrong way: it is per-object configuration", entry->getName());
        else
            throw cRuntimeError("Entry %s= is read from the configuration in the wrong way: it is global (not per-object) configuration", entry->getName());
    }
    if (entry->getType() != requiredType)
        throw cRuntimeError("Entry %s= is read from the configuration with the wrong type (type=%s, actual=%s)",
                            entry->getName(), cConfigOption::getTypeName(requiredType), cConfigOption::getTypeName(entry->getType()));
}

const char *cConfiguration::getAsCustom(cConfigOption *entry, const char *fallbackValue)
{
    assertType(entry, false, cConfigOption::CFG_CUSTOM);
    TRY(return fallback(getConfigValue(entry->getName()), substituteVariables(entry->getDefaultValue()), fallbackValue));
}

bool cConfiguration::getAsBool(cConfigOption *entry, bool fallbackValue)
{
    assertType(entry, false, cConfigOption::CFG_BOOL);
    TRY(return parseBool(getConfigValue(entry->getName()), substituteVariables(entry->getDefaultValue()), fallbackValue));
}

long cConfiguration::getAsInt(cConfigOption *entry, long fallbackValue)
{
    assertType(entry, false, cConfigOption::CFG_INT);
    TRY(return parseLong(getConfigValue(entry->getName()), substituteVariables(entry->getDefaultValue()), fallbackValue));
}

double cConfiguration::getAsDouble(cConfigOption *entry, double fallbackValue)
{
    assertType(entry, false, cConfigOption::CFG_DOUBLE);
    TRY(return parseDouble(getConfigValue(entry->getName()), entry->getUnit(), substituteVariables(entry->getDefaultValue()), fallbackValue));
}

std::string cConfiguration::getAsString(cConfigOption *entry, const char *fallbackValue)
{
    assertType(entry, false, cConfigOption::CFG_STRING);
    TRY(return parseString(getConfigValue(entry->getName()), substituteVariables(entry->getDefaultValue()), fallbackValue));
}

std::string cConfiguration::getAsFilename(cConfigOption *entry)
{
    assertType(entry, false, cConfigOption::CFG_FILENAME);
    const KeyValue& keyvalue = getConfigEntry(entry->getName());
    TRY(return parseFilename(keyvalue.getValue(), keyvalue.getBaseDirectory(), substituteVariables(entry->getDefaultValue())));
}

std::vector<std::string> cConfiguration::getAsFilenames(cConfigOption *entry)
{
    assertType(entry, false, cConfigOption::CFG_FILENAMES);
    const KeyValue& keyvalue = getConfigEntry(entry->getName());
    TRY(return parseFilenames(keyvalue.getValue(), keyvalue.getBaseDirectory(), substituteVariables(entry->getDefaultValue())));
}

std::string cConfiguration::getAsPath(cConfigOption *entry)
{
    assertType(entry, false, cConfigOption::CFG_PATH);
    const KeyValue& keyvalue = getConfigEntry(entry->getName());
    TRY(return adjustPath(keyvalue.getValue(), keyvalue.getBaseDirectory(), substituteVariables(entry->getDefaultValue())));
}

//----

const char *cConfiguration::getAsCustom(const char *objectFullPath, cConfigOption *entry, const char *fallbackValue)
{
    assertType(entry, true, cConfigOption::CFG_CUSTOM);
    TRY(return fallback(getPerObjectConfigValue(objectFullPath, entry->getName()), substituteVariables(entry->getDefaultValue()), fallbackValue));
}

bool cConfiguration::getAsBool(const char *objectFullPath, cConfigOption *entry, bool fallbackValue)
{
    assertType(entry, true, cConfigOption::CFG_BOOL);
    TRY(return parseBool(getPerObjectConfigValue(objectFullPath, entry->getName()), substituteVariables(entry->getDefaultValue()), fallbackValue));
}

long cConfiguration::getAsInt(const char *objectFullPath, cConfigOption *entry, long fallbackValue)
{
    assertType(entry, true, cConfigOption::CFG_INT);
    TRY(return parseLong(getPerObjectConfigValue(objectFullPath, entry->getName()), substituteVariables(entry->getDefaultValue()), fallbackValue));
}

double cConfiguration::getAsDouble(const char *objectFullPath, cConfigOption *entry, double fallbackValue)
{
    assertType(entry, true, cConfigOption::CFG_DOUBLE);
    TRY(return parseDouble(getPerObjectConfigValue(objectFullPath, entry->getName()), entry->getUnit(), substituteVariables(entry->getDefaultValue()), fallbackValue));
}

std::string cConfiguration::getAsString(const char *objectFullPath, cConfigOption *entry, const char *fallbackValue)
{
    assertType(entry, true, cConfigOption::CFG_STRING);
    TRY(return parseString(getPerObjectConfigValue(objectFullPath, entry->getName()), substituteVariables(entry->getDefaultValue()), fallbackValue));
}

std::string cConfiguration::getAsFilename(const char *objectFullPath, cConfigOption *entry)
{
    assertType(entry, true, cConfigOption::CFG_FILENAME);
    const KeyValue& keyvalue = getConfigEntry(entry->getName());
    TRY(return parseFilename(keyvalue.getValue(), keyvalue.getBaseDirectory(), substituteVariables(entry->getDefaultValue())));
}

std::vector<std::string> cConfiguration::getAsFilenames(const char *objectFullPath, cConfigOption *entry)
{
    assertType(entry, true, cConfigOption::CFG_FILENAMES);
    const KeyValue& keyvalue = getConfigEntry(entry->getName());
    TRY(return parseFilenames(keyvalue.getValue(), keyvalue.getBaseDirectory(), substituteVariables(entry->getDefaultValue())));
}

std::string cConfiguration::getAsPath(const char *objectFullPath, cConfigOption *entry)
{
    assertType(entry, true, cConfigOption::CFG_FILENAMES);
    const KeyValue& keyvalue = getConfigEntry(entry->getName());
    TRY(return adjustPath(keyvalue.getValue(), keyvalue.getBaseDirectory(), substituteVariables(entry->getDefaultValue())));
}

