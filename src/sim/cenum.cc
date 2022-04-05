//=========================================================================
//  CENUM.CC - part of
//
//                  OMNeT++/OMNEST
//           Discrete System Simulation in C++
//
//   Member functions of
//    cEnum : effective integer-to-string mapping
//
//=========================================================================

/*--------------------------------------------------------------*
  Copyright (C) 1992-2017 Andras Varga
  Copyright (C) 2006-2017 OpenSim Ltd.

  This file is distributed WITHOUT ANY WARRANTY. See the file
  `license' for details on this and other legal matters.
*--------------------------------------------------------------*/

#include <cstdio>  // sprintf, fprintf
#include <sstream>
#include "omnetpp/globals.h"
#include "omnetpp/csimulation.h"
#include "omnetpp/cenum.h"
#include "omnetpp/cexception.h"
#include "omnetpp/cstringtokenizer.h"

using namespace omnetpp::internal;

namespace omnetpp {

Register_Class(cEnum);

cEnum::cEnum(const cEnum& other) : cOwnedObject(other)
{
    copy(other);
}

cEnum::cEnum(const char *name) : cOwnedObject(name, false)
{
}

cEnum::cEnum(const char *name, const char *str, ...) : cOwnedObject(name, false)
{
    va_list va;
    va_start(va, str);

    const char *s = str;
    int v;
    while (s) {
        v = va_arg(va, int);
        insert(v, s);
        s = va_arg(va, const char *);
    }

    va_end(va);
}

void cEnum::copy(const cEnum& other)
{
    valueToNameMap = other.valueToNameMap;
    nameToValueMap = other.nameToValueMap;
}

cEnum& cEnum::operator=(const cEnum& other)
{
    if (this == &other)
        return *this;
    cOwnedObject::operator=(other);
    copy(other);
    return *this;
}

void cEnum::insert(int value, const char *name)
{
    valueToNameMap[value] = name;
    nameToValueMap[name] = value;
}

void cEnum::bulkInsert(const char *name1, ...)
{
    va_list va;
    va_start(va, name1);

    const char *s = name1;
    int v;
    while (s) {
        v = va_arg(va, int);
        insert(v, s);
        s = va_arg(va, const char *);
    }

    va_end(va);
}

const char *cEnum::getStringFor(int value)
{
    std::map<int, std::string>::const_iterator it = valueToNameMap.find(value);
    return it == valueToNameMap.end() ? nullptr : it->second.c_str();
}

int cEnum::lookup(const char *name, int fallback)
{
    std::map<std::string, int>::const_iterator it = nameToValueMap.find(name);
    return it == nameToValueMap.end() ? fallback : it->second;
}

int cEnum::resolve(const char *name)
{
    std::map<std::string, int>::const_iterator it = nameToValueMap.find(name);
    if (it == nameToValueMap.end())
        throw cRuntimeError("Symbol \"%s\" not found in enum \"%s\"", name, getName());
    return it->second;
}

cEnum *cEnum::find(const char *enumName, const char *contextNamespace)
{
    return dynamic_cast<cEnum *>(enums.getInstance()->lookup(enumName, contextNamespace));
}

cEnum *cEnum::get(const char *enumName, const char *contextNamespace)
{
    cEnum *p = find(enumName, contextNamespace);
    if (!p)
        throw cRuntimeError("Enum \"%s\" not found -- its declaration may be missing "
                            "from .msg files, or the code was not linked in", enumName);
    return p;
}

cEnum *cEnum::registerNames(const char *nameList)
{
    tmpNames.clear();
    cStringTokenizer tokenizer(nameList, "(), ");
    while (tokenizer.hasMoreTokens()) {
        const char *token = tokenizer.nextToken();
        while (const char *sep = strstr(token, "::"))
            token = sep + 2;
        tmpNames.push_back(token);
    }
    return this;
}

cEnum *cEnum::registerValues(int firstValue, ...)
{
    ASSERT(!tmpNames.empty());

    va_list va;
    va_start(va, firstValue);
    insert(firstValue, tmpNames[0].c_str());
    for (int i = 1; i < (int)tmpNames.size(); i++) {
        int value = va_arg(va, int);
        insert(value, tmpNames[i].c_str());
    }
    va_end(va);
    tmpNames.clear();
    return this;
}

std::string cEnum::str() const
{
    if (valueToNameMap.size() == 0)
        return std::string("<empty>");

    std::stringstream out;
    for (std::map<std::string, int>::const_iterator it = nameToValueMap.begin(); it != nameToValueMap.end(); ++it) {
        if (it != nameToValueMap.begin())
            out << ", ";
        out << it->first << "=" << it->second;
    }
    return out.str();
}

}  // namespace omnetpp

