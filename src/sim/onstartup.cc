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

cSymTable::~cSymTable()
{
    for (int i=0; i<(int)v.size(); i++)
        dropAndDelete(v[i]);
}

std::string cSymTable::info() const
{
    if (v.empty())
        return std::string("empty");
    std::stringstream out;
    out << "size=" << v.size();
    return out.str();
}

void cSymTable::forEachChild(cVisitor *visitor)
{
    for (int i=0; i<(int)v.size(); i++)
        visitor->visit(v[i]);
}

void cSymTable::add(cOwnedObject *obj)
{
    v.push_back(obj);
    take(obj);
    lookupCache.clear();
}

int cSymTable::size()
{
    return v.size();
}

cOwnedObject *cSymTable::get(int i)
{
    if (i<0 || i>=(int)v.size())
        return NULL;
    return v[i];
}

cOwnedObject *cSymTable::get(const char *name)
{
    for (int i=0; i<(int)v.size(); i++)
        if (!strcmp(v[i]->name(), name))
            return v[i];
    return NULL;
}

inline bool less(cObject *a, cObject *b)
{
    return strcmp(a->fullName(), b->fullName()) < 0;
}

void cSymTable::sort()
{
    std::sort(v.begin(), v.end(), less);
}

cOwnedObject *cSymTable::lookup(const char *qualifiedName)
{
    for (int i=0; i<(int)v.size(); i++)
    {
        const char *fullname = v[i]->fullName();
        if (fullname[0]==qualifiedName[0] && strcmp(fullname, qualifiedName)==0)
            return v[i];
    }
    return NULL;
}

cOwnedObject *cSymTable::lookup(const char *name, const char *contextNamespace)
{
    // try the lookup cache
    std::string namespacePrefix = contextNamespace;
    if (!namespacePrefix.empty())
        namespacePrefix += ".";
    LookupCache::iterator it = lookupCache.find(namespacePrefix+name);
    if (it!=lookupCache.end())
        return it->second;

    // new lookup: do it, then cache the result
    while (true)
    {
        cOwnedObject *obj = lookup((namespacePrefix+name).c_str());
        if (obj)
        {
            lookupCache[namespacePrefix+name] = obj;
            return obj;
        }

        if (namespacePrefix.empty())
            break;

        // discard last namespace element
        namespacePrefix.resize(namespacePrefix.length()-1); // chop "."
        size_t k = namespacePrefix.rfind(".", namespacePrefix.length());
        if (k==std::string::npos)
            namespacePrefix.clear();
        else
            namespacePrefix.resize(k+1);
    }
    return NULL;
}

//----

cRegistrationList::cRegistrationList()
{
    tmpname = NULL;
    inst = NULL;
}

cRegistrationList::cRegistrationList(const char *name)
{
    tmpname = name;
    inst = NULL;
}

cRegistrationList::~cRegistrationList()
{
    delete inst;
}

cSymTable *cRegistrationList::instance()
{
    if (!inst)
    {
        inst = new cSymTable(tmpname);
        inst->removeFromOwnershipTree();
    }
    return inst;
}

void cRegistrationList::clear()
{
    if (inst)
    {
        delete inst;
        inst = NULL;
    }
}

