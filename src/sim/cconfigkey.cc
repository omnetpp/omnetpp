//=========================================================================
//  CCONFIGKEY.CC - part of
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


#include "cconfigkey.h"

USING_NAMESPACE


cConfigKey::cConfigKey(const char *name, bool isPerObject, bool isGlobal, Type type, const char *unit,
                       const char *defaultValue, const char *description) :
    cNoncopyableOwnedObject(name, false)
{
    isPerObject_ = isPerObject;
    isGlobal_ = isGlobal;
    if (type==CFG_TIME) {
        type_ = CFG_DOUBLE;
        unit_ = "s";
    } else {
        type_ = type;
        unit_ = unit ? unit : "";
    }
    if (type_==CFG_BOOL && defaultValue)
        defaultValue = (defaultValue[0]=='0' || defaultValue[0]=='f') ? "false" : "true";
    defaultValue_ = defaultValue ? defaultValue : "";
    description_ = description ? description : "";
}

std::string cConfigKey::info() const
{
    std::stringstream out;
    out << (isPerObject_ ? "per-object " : "");
    out << (isGlobal_ ? "global" : "per-run");
    out << ", type=" << typeName(type_);
    if (!unit_.empty()) out << ", unit=\"" << unit_ << "\"";
    if (!defaultValue_.empty()) out << ", default=\"" << defaultValue_ << "\"";
    if (!description_.empty()) out << ", hint: " << description_;
    return out.str();
}

const char *cConfigKey::typeName(Type type)
{
    switch (type)
    {
        case CFG_BOOL:      return "bool";
        case CFG_INT:       return "int";
        case CFG_DOUBLE:    return "double";
        case CFG_STRING:    return "string";
        case CFG_FILENAME:  return "filename";
        case CFG_FILENAMES: return "filenames";
        case CFG_CUSTOM:    return "custom";
        default:            return "???";
    }
}
