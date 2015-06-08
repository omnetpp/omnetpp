//==========================================================================
//  CREGISTRATIONLIST.CC - part of
//                     OMNeT++/OMNEST
//            Discrete System Simulation in C++
//
//
//  cRegistrationList class
//
//==========================================================================

/*--------------------------------------------------------------*
  Copyright (C) 1992-2015 Andras Varga
  Copyright (C) 2006-2015 OpenSim Ltd.

  This file is distributed WITHOUT ANY WARRANTY. See the file
  `license' for details on this and other legal matters.
*--------------------------------------------------------------*/


#include <sstream>
#include <algorithm>
#include "omnetpp/cregistrationlist.h"
#include "omnetpp/cexception.h"
#include "omnetpp/cownedobject.h"

NAMESPACE_BEGIN

cRegistrationList::~cRegistrationList()
{
    for (int i = 0; i < (int)vec.size(); i++)
        dropAndDelete(vec[i]);
}

std::string cRegistrationList::info() const
{
    if (vec.empty())
        return std::string("empty");
    std::stringstream out;
    out << "size=" << vec.size();
    return out.str();
}

void cRegistrationList::forEachChild(cVisitor *visitor)
{
    for (int i = 0; i < (int)vec.size(); i++)
        visitor->visit(vec[i]);
}

void cRegistrationList::add(cOwnedObject *obj)
{
    take(obj);
    vec.push_back(obj);
    nameMap[obj->getName()] = obj;
    fullnameMap[obj->getFullName()] = obj;
}

cOwnedObject *cRegistrationList::get(int i) const
{
    if (i < 0 || i >= (int)vec.size())
        return nullptr;
    return vec[i];
}

cOwnedObject *cRegistrationList::find(const char *name) const
{
    StringObjectMap::const_iterator it = nameMap.find(name);
    return it == nameMap.end() ? nullptr : it->second;
}

cOwnedObject *cRegistrationList::lookup(const char *qname) const
{
    StringObjectMap::const_iterator it = fullnameMap.find(qname);
    return it == fullnameMap.end() ? nullptr : it->second;
}

inline bool less(cObject *a, cObject *b)
{
    return strcmp(a->getFullName(), b->getFullName()) < 0;
}

void cRegistrationList::sort()
{
    std::sort(vec.begin(), vec.end(), less);
}

//----

cGlobalRegistrationList::cGlobalRegistrationList()
{
    tmpname = nullptr;
    inst = nullptr;
}

cGlobalRegistrationList::cGlobalRegistrationList(const char *name)
{
    tmpname = name;
    inst = nullptr;
}

cGlobalRegistrationList::~cGlobalRegistrationList()
{
    // delete inst; -- this is usually not a good idea, as things may be
    // in an inconsistent state by now; especially if the program is
    // exiting via exit() or abort(), ie. not by returning from main().
}

cRegistrationList *cGlobalRegistrationList::getInstance()
{
    if (!inst)
        inst = new cRegistrationList(tmpname);
    return inst;
}

void cGlobalRegistrationList::clear()
{
    delete inst;
    inst = nullptr;
}

NAMESPACE_END

