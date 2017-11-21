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
#include "exception.h"
#include "msgtypetable.h"
#include "msganalyzer.h"
#include "msgcompiler.h"

using namespace omnetpp::common;

namespace omnetpp {
namespace nedxml {

MsgTypeTable::~MsgTypeTable()
{
    for (ASTNode *node : importedMsgFiles)
        delete node;
}

void MsgTypeTable::initDescriptors()
{
}


MsgTypeTable::StringVector MsgTypeTable::lookupExistingEnumName(const std::string& name, const std::string& contextNamespace)
{
    StringVector ret;

    if (name.empty()) {
        return ret;
    }

    // if $name contains "::" then user means explicitly qualified name; otherwise he means 'in whichever namespace it is'
    if (name.find("::") != name.npos) {
        std::string qname = name.substr(0, 2) == "::" ? name.substr(2) : name;  // remove leading "::"
        if (containsKey(definedEnums, qname)) {
            ret.push_back(qname);
            return ret;
        }
    }
    else {
        std::string qname = prefixWithNamespace(contextNamespace, name);  // prefer name from local namespace
        if (containsKey(definedEnums, qname)) {
            ret.push_back(qname);
            return ret;
        }
    }

    std::string doubleColonPlusName = std::string("::") + name;
    for (auto enumQName : keys(definedEnums)) {
        if (enumQName == name || opp_stringendswith(enumQName.c_str(), doubleColonPlusName.c_str()))
            ret.push_back(enumQName);
    }
    return ret;
}

MsgTypeTable::ClassInfo& MsgTypeTable::getClassInfo(const std::string& qname)
{
    auto it = definedClasses.find(qname);
    Assert(it != definedClasses.end());
    ClassInfo& classInfo = it->second;
    return classInfo;
}

const MsgTypeTable::EnumInfo& MsgTypeTable::getEnumInfo(const std::string& qname)
{
    auto it = definedEnums.find(qname);
    Assert(it != definedEnums.end());
    return it->second;
}

}  // namespace nedxml
}  // namespace omnetpp
