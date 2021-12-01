//=========================================================================
//  ENUMTYPE.CC - part of
//                  OMNeT++/OMNEST
//           Discrete System Simulation in C++
//
//=========================================================================

/*--------------------------------------------------------------*
  Copyright (C) 1992-2017 Andras Varga
  Copyright (C) 2006-2017 OpenSim Ltd.

  This file is distributed WITHOUT ANY WARRANTY. See the file
  `license' for details on this and other legal matters.
*--------------------------------------------------------------*/

#include <sstream>
#include <algorithm>
#include "common/stringutil.h"
#include "scaveutils.h"
#include "enumtype.h"

using namespace omnetpp::common;

namespace omnetpp {
namespace scave {

EnumType::EnumType(const EnumType& list)
{
    copy(list);
}

void EnumType::copy(const EnumType& other)
{
    valueToNameMap = other.valueToNameMap;
    nameToValueMap = other.nameToValueMap;
}

EnumType& EnumType::operator=(const EnumType& other)
{
    if (this == &other)
        return *this;
    copy(other);
    return *this;
}

void EnumType::insert(int value, const char *name)
{
    valueToNameMap[value] = name;
    nameToValueMap[name] = value;
}

const char *EnumType::nameOf(int value) const
{
    std::map<int, std::string>::const_iterator it = valueToNameMap.find(value);
    return it == valueToNameMap.end() ? nullptr : it->second.c_str();
}

int EnumType::valueOf(const char *name, int fallback) const
{
    std::map<std::string, int>::const_iterator it = nameToValueMap.find(name);
    return it == nameToValueMap.end() ? fallback : it->second;
}

static bool less(const std::pair<int, std::string>& left, const std::pair<int, std::string>& right)
{
    return left.first < right.first;
}

static std::string second(std::pair<int, std::string> pair)
{
    return pair.second;
}

std::vector<std::string> EnumType::getNames() const
{
    std::vector<std::pair<int, std::string> > pairs(valueToNameMap.begin(), valueToNameMap.end());
    std::sort(pairs.begin(), pairs.end(), less);
    std::vector<std::string> names(pairs.size());
    std::transform(pairs.begin(), pairs.end(), names.begin(), second);
    return names;
}

std::string EnumType::str() const
{
    std::stringstream out;
    for (std::map<std::string, int>::const_iterator it = nameToValueMap.begin(); it != nameToValueMap.end(); ++it) {
        if (it != nameToValueMap.begin())
            out << ", ";
        out << it->first << "=" << it->second;
    }
    return out.str();
}

void EnumType::parseFromString(const char *str)
{
    valueToNameMap.clear();
    nameToValueMap.clear();

    int value = -1;
    for (std::string nameValue : opp_splitandtrim(str, ",")) {
        std::string::size_type pos = nameValue.find('=');
        if (pos == std::string::npos) {
            insert(++value, nameValue.c_str());
        }
        else {
            std::string name = nameValue.substr(0, pos);
            std::string valueStr = nameValue.substr(pos+1);
            if (!parseInt(valueStr.c_str(), value))
                throw opp_runtime_error("Enum value must be an int, found: %s", valueStr.c_str());
            insert(value, name.c_str());
        }
    }
}

}  // namespace scave
}  // namespace omnetpp

