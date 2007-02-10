//=========================================================================
//  CCONFIGENTRY.CC - part of
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


#include "cconfigentry.h"


cConfigEntry::cConfigEntry(const char *name, const char *section, bool isGlobal, Type type,
                           const char *unit, const char *defaultValue, const char *description) :
cNoncopyableOwnedObject(name)
{
    section_ = section;
    isGlobal_ = isGlobal;
    if (type==CFG_TIME) {
        type_ = CFG_DOUBLE;
        unit_ = "s";
    } else {
        type_ = type;
        unit_ = unit ? unit : "";
    }
    defaultValue_ = defaultValue ? defaultValue : "";
    description_ = description ? description : "";
}

const char *cConfigEntry::fullName() const
{
    fullname_ = std::string("[") + section_ + "] " + name();
    return fullname_.c_str();
}

std::string cConfigEntry::info() const
{
    const char *type;
    switch (type_)
    {
      case CFG_BOOL:      type = "bool"; break;
      case CFG_INT:       type = "int"; break;
      case CFG_DOUBLE:    type = "double"; break;
      case CFG_STRING:    type = "string"; break;
      case CFG_FILENAME:  type = "filename"; break;
      case CFG_FILENAMES: type = "filenames"; break;
      case CFG_CUSTOM:    type = "custom"; break;
      default:            type = "???";
    }
    std::stringstream out;
    out << (isGlobal_ ? "global" : "per-run");
    out << ", type=" << type;
    if (!unit_.empty()) out << ", unit=\"" << unit_ << "\"";
    if (!defaultValue_.empty()) out << ", default=\"" << defaultValue_ << "\"";
    if (!description_.empty()) out << ", hint: " << description_;
    return out.str();
}

