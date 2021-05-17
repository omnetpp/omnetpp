//==========================================================================
//  CPROPERTIES.CC - part of
//                     OMNeT++/OMNEST
//            Discrete System Simulation in C++
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
#include "common/stringutil.h"
#include "omnetpp/cproperties.h"
#include "omnetpp/cproperty.h"

using namespace omnetpp::common;

namespace omnetpp {

cProperties::~cProperties()
{
    for (auto & property : properties)
        delete property;
}

void cProperties::copy(const cProperties& other)
{
    if (isLocked)
        throw cRuntimeError(this, E_LOCKED);

    // note: do NOT copy islocked flag

    // delete existing contents
    for (auto & property : properties)
        delete property;
    properties.clear();

    // copy properties from other
    for (auto property : other.properties) {
        cProperty *p = property->dup();
        properties.push_back(p);
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
    if (properties.empty())
        return "";
    std::stringstream out;
    int numProperties = (int)properties.size();
    int numDisplayed = std::min(3, numProperties);
    for (int i = 0; i < numDisplayed; i++)
        out << (i == 0 ? "@" : ", @") << properties[i]->getFullName();
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
    if (k < 0 || k >= (int)properties.size())
        throw cRuntimeError(this, "Property index %d out of range", k);
    return properties[k];
}

cProperty *cProperties::get(const char *name, const char *index) const
{
    for (auto property : properties)
        if (!strcmp(property->getName(), name) && !opp_strcmp(index, property->getIndex()))
            return property;
    return nullptr;
}

bool cProperties::getAsBool(const char *name, const char *index) const
{
    cProperty *prop = get(name, index);
    if (!prop)
        return false;
    const char *value = prop->getValue(cProperty::DEFAULTKEY, 0);
    if (!opp_isempty(value) && strcmp(value, "true") != 0 && strcmp(value, "false") != 0)
        throw cRuntimeError(this, "@%s property: Boolean value expected, got '%s'", name, value);

    return opp_strcmp(value, "false") == 0 ? false : true;
}

void cProperties::add(cProperty *p)
{
    if (isLocked)
        throw cRuntimeError(this, E_LOCKED);
    properties.push_back(p);
    p->setOwner(this);
}

void cProperties::remove(int k)
{
    if (isLocked)
        throw cRuntimeError(this, E_LOCKED);

    if (k < 0 || k >= (int)properties.size())
        throw cRuntimeError(this, "Property index %d out of range", k);

    delete properties[k];
    properties.erase(properties.begin() + k);
}

const std::vector<const char *> cProperties::getNames() const
{
    std::vector<const char *> v;
    for (auto i : properties) {
        const char *s = i->getName();
        if (std::find(v.begin(), v.end(), s) == v.end())
            v.push_back(s);
    }
    return v;
}

std::vector<const char *> cProperties::getIndicesFor(const char *name) const
{
    std::vector<const char *> v;
    for (auto property : properties) {
        if (!strcmp(property->getName(), name)) {
            const char *s = property->getIndex();
            if (std::find(v.begin(), v.end(), s) == v.end())
                v.push_back(s);
        }
    }
    return v;
}

void cProperties::lock()
{
    isLocked = true;
    for (auto & property : properties)
        property->lock();
}

}  // namespace omnetpp

