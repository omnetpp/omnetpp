//==========================================================================
//  ONSTARTUP.CC - part of
//                     OMNeT++/OMNEST
//            Discrete System Simulation in C++
//
//
//  supporting class for EXECUTE_ON_STARTUP macro
//
//==========================================================================

/*--------------------------------------------------------------*
  Copyright (C) 1992-2005 Andras Varga

  This file is distributed WITHOUT ANY WARRANTY. See the file
  `license' for details on this and other legal matters.
*--------------------------------------------------------------*/


#include <algorithm>
#include "onstartup.h"
#include "cexception.h"
#include "carray.h"

USING_NAMESPACE


ExecuteOnStartup *ExecuteOnStartup::head;


ExecuteOnStartup::ExecuteOnStartup(void (*_code_to_exec)())
{
    code_to_exec = _code_to_exec;

    // add to list
    next = head;
    head = this;
}


ExecuteOnStartup::~ExecuteOnStartup()
{
}


void ExecuteOnStartup::execute()
{
    code_to_exec();
}

void ExecuteOnStartup::executeAll()
{
    ExecuteOnStartup *p = ExecuteOnStartup::head;
    while (p)
    {
        p->execute();
        p = p->next;
    }

    // null out list to prevent double execution on subsequent calls (e.g. after dll loading)
    ExecuteOnStartup::head = NULL;
}

//----

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
    // delete inst; -- this is a bad idea, easy to cause crash in cStringPool when simulation
    // exits via exit() or abort(), ie. not by normally returning from main().
}

cRegistrationList *cGlobalRegistrationList::instance()
{
    if (!inst)
        inst = new cRegistrationList(tmpname);
    return inst;
}

void cGlobalRegistrationList::clear()
{
    if (inst)
    {
        delete inst;
        inst = NULL;
    }
}

