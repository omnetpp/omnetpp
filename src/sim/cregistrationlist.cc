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
  Copyright (C) 1992-2017 Andras Varga
  Copyright (C) 2006-2017 OpenSim Ltd.

  This file is distributed WITHOUT ANY WARRANTY. See the file
  `license' for details on this and other legal matters.
*--------------------------------------------------------------*/


#include <sstream>
#include <algorithm>
#include "omnetpp/cregistrationlist.h"
#include "omnetpp/cexception.h"
#include "omnetpp/cownedobject.h"
#include "common/stringutil.h"

using namespace omnetpp::common;

namespace omnetpp {

cRegistrationList::~cRegistrationList()
{
    for (auto & obj : vec)
        dropAndDelete(obj);
}

std::string cRegistrationList::str() const
{
    if (vec.empty())
        return std::string("empty");
    std::stringstream out;
    out << "size=" << vec.size();
    return out.str();
}

void cRegistrationList::forEachChild(cVisitor *visitor)
{
    for (auto & obj : vec)
        if (!visitor->visit(obj))
            return;
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

cOwnedObject *cRegistrationList::lookup(const char *qname, const char *contextNamespace, bool fallbackToOmnetpp)
{
    if (qname[0] == ':' && qname[1] == ':')
        return lookup(qname + 2);

    // try with contextNamespace
    if (!opp_isempty(contextNamespace)) {
        std::string ns = contextNamespace;
        while (!ns.empty()) {
            cOwnedObject *result = lookup((ns + "::" + qname).c_str());
            if (result)
                return result;
            // remove last segment of namespace, and try again
            int pos = ns.rfind("::");
            if (pos == (int)std::string::npos)
                break;
            ns = ns.substr(0, pos);
        }
    }

    // try in the global namespace, then in the omnetpp namespace if requested
    cOwnedObject *result = lookup(qname);
    if (!result && fallbackToOmnetpp)
        result = lookup((std::string("omnetpp::") + qname).c_str());
    return result;
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

cGlobalRegistrationList::cGlobalRegistrationList(const char *name) : tmpname(name)
{
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

}  // namespace omnetpp

