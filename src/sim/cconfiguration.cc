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
  Copyright (C) 2003-2005 Andras Varga

  This file is distributed WITHOUT ANY WARRANTY. See the file
  `license' for details on this and other legal matters.
*--------------------------------------------------------------*/


#include <errno.h>
#include "cconfiguration.h"
#include "cconfigkey.h"
#include "unitconversion.h"
#include "stringutil.h"
#include "fileutil.h"
#include "fnamelisttokenizer.h"
#include "exception.h"

//XXX move these functions into another base class, like cConfigurationBase : public cConfiguration {} ?

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
    //FIXME parse and evaluate!!! using common lib's Expression class.
    if (!s)
        s = defaultValue;
    if (!s)
        return fallbackValue;
    char *endp;
    long d = strtol(s, &endp, 0);
    if (s==endp || *endp)
        throw opp_runtime_error("`%s' is not a valid integer value", s);
    if (errno==ERANGE)
        throw opp_runtime_error("overflow during conversion of `%s'", s);
    return d;
}

double cConfiguration::parseDouble(const char *s, const char *unit, const char *defaultValue, double fallbackValue)
{
    //FIXME parse and evaluate!!! using common lib's Expression class.
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
    if (!s || !s[0])
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

//---

#define TRY(CODE)   try { CODE; } catch (std::exception& e) {throw cRuntimeError("Error getting entry %s= from the configuration: %s", name(), e.what());}

inline const char *fallback(const char *s, const char *defaultValue, const char *fallbackValue)
{
    return s!=NULL ? s : defaultValue!=NULL ? defaultValue : fallbackValue;
}

static void assertType(cConfigKey *entry, bool isPerObject, cConfigKey::Type requiredType)
{
    if (entry->isPerObject() != isPerObject)
    {
        if (entry->isPerObject())
            throw cRuntimeError("Entry %s= is read from the configuration in the wrong way: it is per-object configuration", entry->name());
        else
            throw cRuntimeError("Entry %s= is read from the configuration in the wrong way: it is global (not per-object) configuration", entry->name());
    }
    if (entry->type() != requiredType)
        throw cRuntimeError("Entry %s= is read from the configuration with the wrong type (type=%s, actual=%s)",
                            entry->name(), cConfigKey::typeName(requiredType), cConfigKey::typeName(entry->type()));
}

const char *cConfiguration::getAsCustom(cConfigKey *entry, const char *fallbackValue)
{
    assertType(entry, false, cConfigKey::CFG_CUSTOM);
    TRY(return fallback(getConfigValue(entry->name()), substituteVariables(entry->defaultValue()), fallbackValue));
}

bool cConfiguration::getAsBool(cConfigKey *entry, bool fallbackValue)
{
    assertType(entry, false, cConfigKey::CFG_BOOL);
    TRY(return parseBool(getConfigValue(entry->name()), substituteVariables(entry->defaultValue()), fallbackValue));
}

long cConfiguration::getAsInt(cConfigKey *entry, long fallbackValue)
{
    assertType(entry, false, cConfigKey::CFG_INT);
    TRY(return parseLong(getConfigValue(entry->name()), substituteVariables(entry->defaultValue()), fallbackValue));
}

double cConfiguration::getAsDouble(cConfigKey *entry, double fallbackValue)
{
    assertType(entry, false, cConfigKey::CFG_DOUBLE);
    TRY(return parseDouble(getConfigValue(entry->name()), entry->unit(), substituteVariables(entry->defaultValue()), fallbackValue));
}

std::string cConfiguration::getAsString(cConfigKey *entry, const char *fallbackValue)
{
    assertType(entry, false, cConfigKey::CFG_STRING);
    TRY(return parseString(getConfigValue(entry->name()), substituteVariables(entry->defaultValue()), fallbackValue));
}

std::string cConfiguration::getAsFilename(cConfigKey *entry)
{
    assertType(entry, false, cConfigKey::CFG_FILENAME);
    const KeyValue& keyvalue = getConfigEntry(entry->name());
    TRY(return parseFilename(keyvalue.getValue(), keyvalue.getBaseDirectory(), substituteVariables(entry->defaultValue())));
}

std::vector<std::string> cConfiguration::getAsFilenames(cConfigKey *entry)
{
    assertType(entry, false, cConfigKey::CFG_FILENAMES);
    const KeyValue& keyvalue = getConfigEntry(entry->name());
    TRY(return parseFilenames(keyvalue.getValue(), keyvalue.getBaseDirectory(), substituteVariables(entry->defaultValue())));
}

//----

const char *cConfiguration::getAsCustom(const char *objectFullPath, cConfigKey *entry, const char *fallbackValue)
{
    assertType(entry, true, cConfigKey::CFG_CUSTOM);
    TRY(return fallback(getPerObjectConfigValue(objectFullPath, entry->name()), substituteVariables(entry->defaultValue()), fallbackValue));
}

bool cConfiguration::getAsBool(const char *objectFullPath, cConfigKey *entry, bool fallbackValue)
{
    assertType(entry, true, cConfigKey::CFG_BOOL);
    TRY(return parseBool(getPerObjectConfigValue(objectFullPath, entry->name()), substituteVariables(entry->defaultValue()), fallbackValue));
}

long cConfiguration::getAsInt(const char *objectFullPath, cConfigKey *entry, long fallbackValue)
{
    assertType(entry, true, cConfigKey::CFG_INT);
    TRY(return parseLong(getPerObjectConfigValue(objectFullPath, entry->name()), substituteVariables(entry->defaultValue()), fallbackValue));
}

double cConfiguration::getAsDouble(const char *objectFullPath, cConfigKey *entry, double fallbackValue)
{
    assertType(entry, true, cConfigKey::CFG_DOUBLE);
    TRY(return parseDouble(getPerObjectConfigValue(objectFullPath, entry->name()), entry->unit(), substituteVariables(entry->defaultValue()), fallbackValue));
}

std::string cConfiguration::getAsString(const char *objectFullPath, cConfigKey *entry, const char *fallbackValue)
{
    assertType(entry, true, cConfigKey::CFG_STRING);
    TRY(return parseString(getPerObjectConfigValue(objectFullPath, entry->name()), substituteVariables(entry->defaultValue()), fallbackValue));
}

std::string cConfiguration::getAsFilename(const char *objectFullPath, cConfigKey *entry)
{
    assertType(entry, true, cConfigKey::CFG_FILENAME);
    const KeyValue& keyvalue = getConfigEntry(entry->name());
    TRY(return parseFilename(keyvalue.getValue(), keyvalue.getBaseDirectory(), substituteVariables(entry->defaultValue())));
}

std::vector<std::string> cConfiguration::getAsFilenames(const char *objectFullPath, cConfigKey *entry)
{
    assertType(entry, true, cConfigKey::CFG_FILENAMES);
    const KeyValue& keyvalue = getConfigEntry(entry->name());
    TRY(return parseFilenames(keyvalue.getValue(), keyvalue.getBaseDirectory(), substituteVariables(entry->defaultValue())));
}

