//=========================================================================
//  CCONFIGOPTION.CC - part of
//
//                  OMNeT++/OMNEST
//           Discrete System Simulation in C++
//
//=========================================================================

/*--------------------------------------------------------------*
  Copyright (C) 2003-2017 Andras Varga
  Copyright (C) 2006-2017 OpenSim Ltd.

  This file is distributed WITHOUT ANY WARRANTY. See the file
  `license' for details on this and other legal matters.
*--------------------------------------------------------------*/

#include <cstring>
#include <sstream>
#include "common/exception.h"
#include "omnetpp/cconfigoption.h"
#include "omnetpp/globals.h"

using namespace omnetpp::common;
using namespace omnetpp::internal;

namespace omnetpp {


cConfigOption::cConfigOption(const char *name, bool isGlobal, Type type, const char *unit,
                             const char *defaultValue, const char *description):
    cNoncopyableOwnedObject(name, false),
    isPerObject_(false), isGlobal_(isGlobal), objectKind(KIND_NONE), type(type), unit(unit ? unit : "")
{
    if (type==CFG_BOOL && defaultValue)
        defaultValue = (defaultValue[0]=='0' || defaultValue[0]=='f') ? "false" : "true";
    this->defaultValue = defaultValue ? defaultValue : "";
    this->description = description ? description : "";
}

cConfigOption::cConfigOption(const char *name, ObjectKind kind, Type type, const char *unit,
                       const char *defaultValue, const char *description):
    cNoncopyableOwnedObject(name, false),
    isPerObject_(true), isGlobal_(false), objectKind(kind), type(type), unit(unit ? unit : "")
{
    if (type==CFG_BOOL && defaultValue)
        defaultValue = (defaultValue[0]=='0' || defaultValue[0]=='f') ? "false" : "true";
    this->defaultValue = defaultValue ? defaultValue : "";
    this->description = description ? description : "";

    // per-object config option names must contain hyphen, except for "typename" (named so for consistency with NED)
    if (strchr(name,'-')==nullptr && strcmp(name, "typename")!=0)
        throw opp_runtime_error("Per-object config option name must contain hyphen, check Register_PerObjectConfigOption() macros: %s", name);
}

std::string cConfigOption::str() const
{
    std::stringstream out;
    out << (isPerObject_ ? "per-object " : "");
    out << (isGlobal_ ? "global" : "per-run");
    if (isPerObject_) out << ", object-kind=\"" << getObjectKindName(objectKind) << "\"";
    out << ", type=" << getTypeName(type);
    if (!unit.empty()) out << ", unit=\"" << unit << "\"";
    if (!defaultValue.empty()) out << ", default=\"" << defaultValue << "\"";
    if (!description.empty()) out << ", hint: " << description;
    return out.str();
}

const char *cConfigOption::getTypeName(Type type)
{
    switch (type) {
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
    switch (kind) {
        case KIND_COMPONENT:        return "component";
        case KIND_CHANNEL:          return "channel";
        case KIND_MODULE:           return "module";
        case KIND_SIMPLE_MODULE:    return "simple-module";
        case KIND_UNSPECIFIED_TYPE: return "unspecified-type";
        case KIND_PARAMETER:        return "parameter";
        case KIND_STATISTIC:        return "statistic";
        case KIND_SCALAR:           return "scalar";
        case KIND_VECTOR:           return "vector";
        case KIND_NONE:             return "";
        default:                    return "???";
    }
}

cConfigOption *cConfigOption::find(const char *name)
{
    return dynamic_cast<cConfigOption *>(configOptions.getInstance()->lookup(name));
}

cConfigOption *cConfigOption::get(const char *name)
{
    cConfigOption *configOption = find(name);
    if (!configOption)
        throw cRuntimeError("Configuration option \"%s\" not found", name);
    return configOption;
}

}  // namespace omnetpp

