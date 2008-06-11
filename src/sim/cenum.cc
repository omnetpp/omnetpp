//=========================================================================
//  CENUM.CC - part of
//
//                  OMNeT++/OMNEST
//           Discrete System Simulation in C++
//
//   Member functions of
//    cEnum : effective integer-to-string mapping
//
//  Author: Andras Varga
//
//=========================================================================

/*--------------------------------------------------------------*
  Copyright (C) 1992-2008 Andras Varga
  Copyright (C) 2006-2008 OpenSim Ltd.

  This file is distributed WITHOUT ANY WARRANTY. See the file
  `license' for details on this and other legal matters.
*--------------------------------------------------------------*/

#include <stdio.h>           // sprintf, fprintf
#include <string.h>          // memcmp, memcpy, memset
#include "globals.h"
#include "csimulation.h"
#include "cenum.h"
#include "cexception.h"
#include "cstringtokenizer.h"

USING_NAMESPACE


Register_Class(cEnum);

cEnum::cEnum(const cEnum& list) : cOwnedObject()
{
     setName(list.getName());
     operator=(list);
}

cEnum::cEnum(const char *name) : cOwnedObject(name, false)
{
}

cEnum::~cEnum()
{
}

cEnum& cEnum::operator=(const cEnum& other)
{
    cOwnedObject::operator=(other);
    valueToNameMap = other.valueToNameMap;
    nameToValueMap = other.nameToValueMap;
    return *this;
}

std::string cEnum::info() const
{
    if (valueToNameMap.size()==0)
        return std::string("empty");
    return str();
}

void cEnum::insert(int value, const char *name)
{
    valueToNameMap[value] = name;
    nameToValueMap[name] = value;
}

const char *cEnum::getStringFor(int value)
{
    std::map<int,std::string>::const_iterator it = valueToNameMap.find(value);
    return it==valueToNameMap.end() ? NULL : it->second.c_str();
}

int cEnum::lookup(const char *name, int fallback)
{
    std::map<std::string,int>::const_iterator it = nameToValueMap.find(name);
    return it==nameToValueMap.end() ? fallback : it->second;
}

cEnum *cEnum::find(const char *name)
{
    return dynamic_cast<cEnum *>(enums.getInstance()->lookup(name));
}

cEnum *cEnum::registerNames(const char *nameList)
{
    tmpNames.clear();
    cStringTokenizer tokenizer(nameList, "(), ");
    while (tokenizer.hasMoreTokens())
    {
        const char *token = tokenizer.nextToken();
        if (strstr(token, "::"))
            token = strstr(token, "::")+2;
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
    for (int i=1; i < (int)tmpNames.size(); i++)
    {
        int value = va_arg(va, int);
        insert(value, tmpNames[i].c_str());
    }
    va_end(va);
    tmpNames.clear();
    return this;
}

std::string cEnum::str() const
{
    std::stringstream out;
    for (std::map<std::string,int>::const_iterator it=nameToValueMap.begin(); it!=nameToValueMap.end(); ++it)
    {
        if (it!=nameToValueMap.begin())
            out << ", ";
        out << it->first << "=" << it->second;
    }
    return out.str();
}


