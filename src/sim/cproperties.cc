//==========================================================================
//  CPROPERTIES.CC - part of
//                     OMNeT++/OMNEST
//            Discrete System Simulation in C++
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
#include "common/stringutil.h"
#include "omnetpp/cproperties.h"
#include "omnetpp/cproperty.h"

using namespace omnetpp::common;

namespace omnetpp {

cProperties::cProperties()
{
    isLocked = false;
    refCount = 0;
}

cProperties::~cProperties()
{
    for (int i = 0; i < (int)propv.size(); i++)
        delete propv[i];
}

void cProperties::copy(const cProperties& other)
{
    if (isLocked)
        throw cRuntimeError(this, E_LOCKED);

    // note: do NOT copy islocked flag

    // delete existing contents
    for (int i = 0; i < (int)propv.size(); i++)
        delete propv[i];
    propv.clear();

    // copy properties from other
    for (int i = 0; i < (int)other.propv.size(); i++) {
        cProperty *p = other.propv[i]->dup();
        propv.push_back(p);
    }
}

cProperties& cProperties::operator=(const cProperties& other)
{
    if (this == &other)
        return *this;
    copy(other);
    return *this;
}

std::string cProperties::str() const
{
    if (propv.empty())
        return "";
    std::stringstream out;
    int numProperties = (int)propv.size();
    int numDisplayed = std::min(3, numProperties);
    for (int i = 0; i < numDisplayed; i++)
        out << (i == 0 ? "@" : ", @") << propv[i]->getFullName();
    if (numDisplayed != numProperties)
        out << "... (and " << (numProperties - numDisplayed) << " more)";
    return out.str();
}

void cProperties::parsimPack(cCommBuffer *buffer) const
{
    // TBD
}

void cProperties::parsimUnpack(cCommBuffer *buffer)
{
    if (isLocked)
        throw cRuntimeError(this, E_LOCKED);
    // TBD
}

cProperty *cProperties::get(int k) const
{
    if (k < 0 || k >= (int)propv.size())
        throw cRuntimeError(this, "property index %d out of range", k);
    return propv[k];
}

cProperty *cProperties::get(const char *name, const char *index) const
{
    for (int i = 0; i < (int)propv.size(); i++)
        if (!strcmp(propv[i]->getName(), name) && !omnetpp::opp_strcmp(index, propv[i]->getIndex()))
            return propv[i];

    return nullptr;
}

bool cProperties::getAsBool(const char *name, const char *index) const
{
    cProperty *prop = get(name, index);
    if (!prop)
        return false;
    const char *value = prop->getValue(cProperty::DEFAULTKEY, 0);
    if (!opp_isempty(value) && strcmp(value, "true") != 0 && strcmp(value, "false") != 0)
        throw cRuntimeError(this, "@%s property: boolean value expected, got '%s'", name, value);

    return omnetpp::opp_strcmp(value, "false") == 0 ? false : true;
}

void cProperties::add(cProperty *p)
{
    if (isLocked)
        throw cRuntimeError(this, E_LOCKED);
    propv.push_back(p);
    p->setOwner(this);
}

void cProperties::remove(int k)
{
    if (isLocked)
        throw cRuntimeError(this, E_LOCKED);

    if (k < 0 || k >= (int)propv.size())
        throw cRuntimeError(this, "property index %d out of range", k);

    delete propv[k];
    propv.erase(propv.begin() + k);
}

const std::vector<const char *> cProperties::getNames() const
{
    std::vector<const char *> v;
    for (int i = 0; i < (int)propv.size(); i++) {
        const char *s = propv[i]->getName();
        if (std::find(v.begin(), v.end(), s) == v.end())
            v.push_back(s);
    }
    return v;
}

std::vector<const char *> cProperties::getIndicesFor(const char *name) const
{
    std::vector<const char *> v;
    for (int i = 0; i < (int)propv.size(); i++) {
        if (!strcmp(propv[i]->getName(), name)) {
            const char *s = propv[i]->getIndex();
            if (std::find(v.begin(), v.end(), s) == v.end())
                v.push_back(s);
        }
    }
    return v;
}

void cProperties::lock()
{
    isLocked = true;
    for (int i = 0; i < (int)propv.size(); i++)
        propv[i]->lock();
}

}  // namespace omnetpp

