//=========================================================================
//  CCONFIGOPTION.CC - part of
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

#include <sstream>
#include "cconfigoption.h"
#include "exception.h"

NAMESPACE_BEGIN


cConfigOption::cConfigOption(const char *name, bool isGlobal, Type type, const char *unit,
                       const char *defaultValue, const char *description) :
    cNoncopyableOwnedObject(name, false)
{
    isPerObject_ = false;
    isGlobal_ = isGlobal;
    objectKind_ = KIND_NONE;
    type_ = type;
    unit_ = unit ? unit : "";
    if (type_==CFG_BOOL && defaultValue)
        defaultValue = (defaultValue[0]=='0' || defaultValue[0]=='f') ? "false" : "true";
    defaultValue_ = defaultValue ? defaultValue : "";
    description_ = description ? description : "";
}

cConfigOption::cConfigOption(const char *name, ObjectKind kind, Type type, const char *unit,
                       const char *defaultValue, const char *description) :
    cNoncopyableOwnedObject(name, false)
{
    isPerObject_ = true;
    isGlobal_ = false;
    objectKind_ = kind;
    type_ = type;
    unit_ = unit ? unit : "";
    if (type_==CFG_BOOL && defaultValue)
        defaultValue = (defaultValue[0]=='0' || defaultValue[0]=='f') ? "false" : "true";
    defaultValue_ = defaultValue ? defaultValue : "";
    description_ = description ? description : "";

    // per-object config option names must contain hyphen, except for "typename" (named so for consistency with NED)
    if (strchr(name,'-')==NULL && strcmp(name, "typename")!=0)
        throw opp_runtime_error("Per-object config option name must contain hyphen, check Register_PerObjectConfigOption() macros: %s", name);
}

std::string cConfigOption::info() const
{
    std::stringstream out;
    out << (isPerObject_ ? "per-object " : "");
    out << (isGlobal_ ? "global" : "per-run");
    if (isPerObject_) out << ", object-kind=\"" << getObjectKindName(objectKind_) << "\"";
    out << ", type=" << getTypeName(type_);
    if (!unit_.empty()) out << ", unit=\"" << unit_ << "\"";
    if (!defaultValue_.empty()) out << ", default=\"" << defaultValue_ << "\"";
    if (!description_.empty()) out << ", hint: " << description_;
    return out.str();
}

const char *cConfigOption::getTypeName(Type type)
{
    switch (type)
    {
        case CFG_BOOL:      return "bool";
        case CFG_INT:       return "int";
        case CFG_DOUBLE:    return "double";
        case CFG_STRING:    return "string";
        case CFG_FILENAME:  return "filename";
        case CFG_FILENAMES: return "filenames";
        case CFG_PATH:      return "path";
        case CFG_CUSTOM:    return "custom";
        default:            return "???";
    }
}

const char *cConfigOption::getObjectKindName(ObjectKind kind)
{
    switch (kind)
    {
        case KIND_MODULE:           return "module";
        case KIND_SIMPLE_MODULE:    return "simplemodule";
        case KIND_UNSPECIFIED_TYPE: return "unspecifiedtype";
        case KIND_PARAMETER:        return "parameter";
        case KIND_STATISTIC:        return "statistic";
        case KIND_SCALAR:           return "scalar";
        case KIND_VECTOR:           return "vector";
        case KIND_NONE:             return "";
        default:                    return "???";
    }
}

NAMESPACE_END

