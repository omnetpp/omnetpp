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
  Copyright (C) 1992-2008 Andras Varga
  Copyright (C) 2006-2008 OpenSim Ltd.

  This file is distributed WITHOUT ANY WARRANTY. See the file
  `license' for details on this and other legal matters.
*--------------------------------------------------------------*/


#include <sstream>
#include <algorithm>
#include "cregistrationlist.h"
#include "cexception.h"
#include "cownedobject.h"

USING_NAMESPACE


cRegistrationList::~cRegistrationList()
{
    for (int i=0; i<(int)vec.size(); i++)
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
    for (int i=0; i<(int)vec.size(); i++)
        visitor->visit(vec[i]);
}

void cRegistrationList::add(cOwnedObject *obj)
{
    take(obj);
    vec.push_back(obj);
    nameMap[obj->name()] = obj;
    fullnameMap[obj->fullName()] = obj;
}

cOwnedObject *cRegistrationList::get(int i) const
{
    if (i<0 || i>=(int)vec.size())
        return NULL;
    return vec[i];
}

cOwnedObject *cRegistrationList::get(const char *name) const
{
    StringObjectMap::const_iterator it = nameMap.find(name);
    return it==nameMap.end() ? NULL : it->second;
}

cOwnedObject *cRegistrationList::lookup(const char *qname) const
{
    StringObjectMap::const_iterator it = fullnameMap.find(qname);
    return it==fullnameMap.end() ? NULL : it->second;
}

inline bool less(cObject *a, cObject *b)
{
    return strcmp(a->fullName(), b->fullName()) < 0;
}

void cRegistrationList::sort()
{
    std::sort(vec.begin(), vec.end(), less);
}

//----

cGlobalRegistrationList::cGlobalRegistrationList()
{
    tmpname = NULL;
    inst = NULL;
}

cGlobalRegistrationList::cGlobalRegistrationList(const char *name)
{
    tmpname = name;
    inst = NULL;
}

cGlobalRegistrationList::~cGlobalRegistrationList()
{
    // delete inst; -- this is usually not a good idea, as things may be
    // in an inconsistent state by now; especially if the program is
    // exiting via exit() or abort(), ie. not by returning from main().
}

cRegistrationList *cGlobalRegistrationList::instance()
{
    if (!inst)
        inst = new cRegistrationList(tmpname);
    return inst;
}

void cGlobalRegistrationList::clear()
{
    delete inst;
    inst = NULL;
}

