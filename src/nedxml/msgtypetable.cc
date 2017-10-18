//==========================================================================
//  MSGTYPETABLE.CC - part of
//
//                     OMNeT++/OMNEST
//            Discrete System Simulation in C++
//
//==========================================================================

/*--------------------------------------------------------------*
  Copyright (C) 2002-2017 Andras Varga
  Copyright (C) 2006-2017 OpenSim Ltd.

  This file is distributed WITHOUT ANY WARRANTY. See the file
  `license' for details on this and other legal matters.
*--------------------------------------------------------------*/

#include "common/stringutil.h"
#include "common/stlutil.h"
#include "nedexception.h"
#include "nedutil.h"
#include "msgtypetable.h"

using namespace omnetpp::common;

namespace omnetpp {
namespace nedxml {

MsgTypeTable::TypeDesc MsgTypeTable::_PRIMITIVE_TYPES[] =
{ //     nedTypeName        cppTypeName        fromstring            tostring               emptyValue
        {"bit",             "bit",             "bit(string2long($))","long2string($.get())","bit(0)"},
        {"bool",            "bool",            "string2bool($)",     "bool2string($)",      "false"},
        {"float",           "float",           "string2double($)",   "double2string($)",    "0"},
        {"double",          "double",          "string2double($)",   "double2string($)",    "0"},
        {"simtime_t", "::omnetpp::simtime_t",  "string2simtime($)",  "simtime2string($)",   "0"},
        {"string",    "::omnetpp::opp_string", "($)",                "oppstring2string($)", ""},
        {"char",            "char",            "string2long($)",     "long2string($)",      "0"},
        {"short",           "short",           "string2long($)",     "long2string($)",      "0"},
        {"int",             "int",             "string2long($)",     "long2string($)",      "0"},
        {"long",            "long",            "string2long($)",     "long2string($)",      "0"},
        {"int8",            "int8_t",          "string2long($)",     "long2string($)",      "0"},
        {"int8_t",          "int8_t",          "string2long($)",     "long2string($)",      "0"},
        {"int16",           "int16_t",         "string2long($)",     "long2string($)",      "0"},
        {"int16_t",         "int16_t",         "string2long($)",     "long2string($)",      "0"},
        {"int32",           "int32_t",         "string2long($)",     "long2string($)",      "0"},
        {"int32_t",         "int32_t",         "string2long($)",     "long2string($)",      "0"},
        {"unsigned char",   "unsigned char",   "string2ulong($)",    "ulong2string($)",     "0"},
        {"unsigned short",  "unsigned short",  "string2ulong($)",    "ulong2string($)",     "0"},
        {"unsigned int",    "unsigned int",    "string2ulong($)",    "ulong2string($)",     "0"},
        {"unsigned long",   "unsigned long",   "string2ulong($)",    "ulong2string($)",     "0"},
        {"uint8",           "uint8_t",         "string2ulong($)",    "ulong2string($)",     "0"},
        {"uint8_t",         "uint8_t",         "string2ulong($)",    "ulong2string($)",     "0"},
        {"uint16",          "uint16_t",        "string2ulong($)",    "ulong2string($)",     "0"},
        {"uint16_t",        "uint16_t",        "string2ulong($)",    "ulong2string($)",     "0"},
        {"uint32",          "uint32_t",        "string2ulong($)",    "ulong2string($)",     "0"},
        {"uint32_t",        "uint32_t",        "string2ulong($)",    "ulong2string($)",     "0"},
        {"int64",           "int64_t",         "string2int64($)",    "int642string($)",     "0"},
        {"int64_t",         "int64_t",         "string2int64($)",    "int642string($)",     "0"},
        {"uint64",          "uint64_t",        "string2uint64($)",   "uint642string($)",    "0"},
        {"uint64_t",        "uint64_t",        "string2uint64($)",   "uint642string($)",    "0"},
        {nullptr,nullptr,nullptr,nullptr,nullptr}
};

MsgTypeTable::~MsgTypeTable()
{
    for (NEDElement *node : importedNedFiles)
        delete node;
}

void MsgTypeTable::initDescriptors()
{
    for (int i = 0; _PRIMITIVE_TYPES[i].nedTypeName; ++i)
        PRIMITIVE_TYPES[_PRIMITIVE_TYPES[i].nedTypeName] = _PRIMITIVE_TYPES[i];

    // pre-register some OMNeT++ classes so that one doesn't need to announce them
    //
    // @classes contains fully qualified names (ie with namespace); keys to the other hashes are fully qualified as well
    //
    // note: $classtype values:
    //  'cownedobject' ==> subclasses from cOwnedObject
    //  'cnamedobject' ==> subclasses from cNamedObject but NOT from cOwnedObject
    //  'cobject'      ==> subclasses from cObject but NOT from cNamedObject
    //  'foreign'      ==> non-cObject class (classes announced as "class noncobject" or "extends void")
    //  'struct'       ==> struct (no member functions)
    //
    classes["omnetpp::cObject"] = ClassType::COBJECT;
    classes["omnetpp::cNamedObject"] = ClassType::CNAMEDOBJECT;
    classes["omnetpp::cOwnedObject"] = ClassType::COWNEDOBJECT;
    classes["omnetpp::cMessage"] = ClassType::COWNEDOBJECT;
    classes["omnetpp::cPacket"] = ClassType::COWNEDOBJECT;
    classes["omnetpp::cModule"] = ClassType::COWNEDOBJECT;
    // TODO: others?
}

MsgTypeTable::StringVector MsgTypeTable::lookupExistingClassName(const std::string& name, const std::string& contextNamespace)
{
    StringVector ret;
    std::map<std::string, ClassType>::iterator it;

    if (name.empty()) {
        ret.push_back(name);
        return ret;
    }

    // if $name contains "::" then user means explicitly qualified name; otherwise he means 'in whichever namespace it is'
    if (name.find("::") != name.npos) {
        std::string qname = name.substr(0, 2) == "::" ? name.substr(2) : name;  // remove leading "::", because names in @classes don't have it either
        it = classes.find(qname);
        if (it != classes.end()) {
            ret.push_back(it->first);
            return ret;
        }
    }
    else {
        std::string qname = prefixWithNamespace(contextNamespace, name);
        it = classes.find(qname);
        if (it != classes.end()) {
            ret.push_back(it->first);
            return ret;
        }
    }
    size_t namelength = name.length();
    for (it = classes.begin(); it != classes.end(); ++it) {
        size_t l = it->first.length();
        if (l >= namelength) {
            size_t pos = l - namelength;
            if ((pos == 0 || it->first[pos-1] == ':') && (it->first.substr(pos) == name)) {
                ret.push_back(it->first);
            }
        }
    }
    return ret;
}

MsgTypeTable::StringVector MsgTypeTable::lookupExistingEnumName(const std::string& name, const std::string& contextNamespace)
{
    StringVector ret;
    std::map<std::string, std::string>::iterator it;

    if (name.empty()) {
        // ret.push_back(name);
        return ret;
    }

    // if $name contains "::" then user means explicitly qualified name; otherwise he means 'in whichever namespace it is'
    if (name.find("::") != name.npos) {
        std::string qname = name.substr(0, 2) == "::" ? name.substr(2) : name;  // remove leading "::", because names in @classes don't have it either
        it = enums.find(qname);
        if (it != enums.end()) {
            ret.push_back(it->second);
            return ret;
        }
    }
    else {
        std::string qname = prefixWithNamespace(contextNamespace, name);  // prefer name from local namespace
        it = enums.find(qname);
        if (it != enums.end()) {
            ret.push_back(it->second);
            return ret;
        }
    }

    size_t namelength = name.length();
    for (it = enums.begin(); it != enums.end(); ++it) {
        size_t l = it->second.length();
        if (l >= namelength) {
            size_t pos = l - namelength;
            if ((pos == 0 || it->second[pos-1] == ':') && (it->second.substr(pos) == name)) {
                ret.push_back(it->second);
            }
        }
    }
    return ret;
}

void MsgTypeTable::addClassType(const std::string& classqname, ClassType type, NEDElement *context)
{
    if (classes.find(classqname) != classes.end()) {
        if (classes[classqname] != type)
            ; //TODO: errors->addError(context, "different declarations for '%s' are inconsistent\n", classqname.c_str());
    }
    else {
        classes[classqname] = type;
    }
}

MsgTypeTable::ClassType MsgTypeTable::getClassType(const std::string& classqname)
{
    Assert(!classqname.empty() && classqname[0] != ':');  // must not start with "::"
    std::map<std::string, ClassType>::iterator it = classes.find(classqname);
    ClassType type = it != classes.end() ? it->second : ClassType::UNKNOWN;
    return type;
}

}  // namespace nedxml
}  // namespace omnetpp
