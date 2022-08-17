//==========================================================================
//  CPROPERTY.CC - part of
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
#include "omnetpp/cproperty.h"
#include "omnetpp/cproperties.h"
#include "common/stringutil.h"

namespace omnetpp {

using namespace common;

const char *cProperty::DEFAULTKEY = "";

cProperty::cProperty(const char *name, const char *index) : cNamedObject(name, true)
{
    setIndex(index);
}

cProperty::~cProperty()
{
}

void cProperty::copy(const cProperty& other)
{
    fullName = nullptr;
    setIndex(other.getIndex());
    keyv = other.keyv;
    valuesv = other.valuesv;
}

cProperty& cProperty::operator=(const cProperty& other)
{
    if (this == &other)
        return *this;
    if (isLocked())
        throw cRuntimeError(this, E_LOCKED);
    cNamedObject::operator=(other);
    setName(other.getName());  // cNamedObject doesn't do that
    copy(other);
    setFlag(FL_ISLOCKED, false);
    return *this;
}

void cProperty::setName(const char *name)
{
    if (isLocked())
        throw cRuntimeError(this, E_LOCKED);
    if (name && name[0] == '@')
        throw cRuntimeError(this, "setName(): Property name must be specified without the '@' character");

    cNamedObject::setName(name);
    fullName = nullptr;
}

const char *cProperty::getFullName() const
{
    if (fullName.c_str() == nullptr) {
        if (indexString.c_str() == nullptr)
            fullName = getName();
        else {
            std::stringstream os;
            os << getName() << "[" << indexString.c_str() << "]";
            fullName = os.str();
        }
    }
    return fullName.c_str();
}

std::string cProperty::str() const
{
    std::stringstream os;
    os << "@" << getFullName();
    if (!keyv.empty()) {
        os << "(";
        printValues(os);
        os << ")";
    }
    return os.str();
}

void cProperty::printValues(std::ostream& os) const
{
    int n = keyv.size();
    for (int i = 0; i < n; i++) {
        if (i != 0)
            os << ";";
        if (!keyv[i].empty())
            os << keyv[i].c_str() << "=";
        for (int j = 0; j < (int)valuesv[i].size(); j++)
            os << (j == 0 ? "" : ",") << valuesv[i][j].value.c_str();  //FIXME value may need quoting?
    }
}

void cProperty::parsimPack(cCommBuffer *buffer) const
{
    throw cRuntimeError(this, E_CANTPACK);
}

void cProperty::parsimUnpack(cCommBuffer *buffer)
{
    if (isLocked())
        throw cRuntimeError(this, E_LOCKED);
    throw cRuntimeError(this, E_CANTPACK);
}

void cProperty::setIndex(const char *newIndex)
{
    if (isLocked())
        throw cRuntimeError(this, E_LOCKED);

    indexString = opp_isempty(newIndex) ? nullptr : newIndex;
    fullName = nullptr;
}

const char *cProperty::getIndex() const
{
    return indexString.c_str();
}

void cProperty::setIsImplicit(bool b)
{
    if (isLocked())
        throw cRuntimeError(this, E_LOCKED);
    setFlag(FL_ISIMPLICIT, b);
}

bool cProperty::isImplicit() const
{
    return flags & FL_ISIMPLICIT;
}

int cProperty::findKey(const char *key) const
{
    auto it = std::find(keyv.cbegin(), keyv.cend(), opp_staticpooledstring(opp_nulltoempty(key)));
    return it == keyv.end() ? -1 : it - keyv.begin();
}

std::vector<const char *> cProperty::getKeys() const
{
    std::vector<const char *> result;
    for (const auto& k : keyv)
        result.push_back(k.c_str());
    return result;
}

bool cProperty::containsKey(const char *key) const
{
    return findKey(key) != -1;
}

void cProperty::addKey(const char *key)
{
    key = opp_nulltoempty(key);
    int k = findKey(key);
    if (k == -1) {
        keyv.push_back(key);
        valuesv.push_back(ValueList());
    }
}

const cProperty::ValueList& cProperty::getValuesVector(const char *key) const
{
    return const_cast<cProperty*>(this)->getValuesVector(key);
}

cProperty::ValueList& cProperty::getValuesVector(const char *key)
{
    key = opp_nulltoempty(key);
    int k = findKey(key);
    if (k == -1)
        throw cRuntimeError(this, "Property has no key named '%s'", key);
    return valuesv[k];
}

int cProperty::getNumValues(const char *key) const
{
    int k = findKey(opp_nulltoempty(key));
    if (k == -1)
        return 0;
    return valuesv[k].size();
}

void cProperty::setNumValues(const char *key, int size)
{
    if (isLocked())
        throw cRuntimeError(this, E_LOCKED);
    getValuesVector(key).resize(size);
}

const char *cProperty::getValue(const char *key, int index) const
{
    if (!key)
        key = "";
    int k = findKey(key);
    if (k == -1)
        return nullptr;
    const ValueList& v = valuesv[k];
    if (index < 0 || index >= (int)v.size())
        return nullptr;
    return v[index].value.c_str();
}

const char *cProperty::getValueOriginFile(const char *key, int index) const
{
    if (!key)
        key = "";
    int k = findKey(key);
    if (k == -1)
        return nullptr;
    const ValueList& v = valuesv[k];
    if (index < 0 || index >= (int)v.size())
        return nullptr;
    return v[index].originFile.c_str();
}

const char *cProperty::getValueOriginType(const char *key, int index) const
{
    if (!key)
        key = "";
    int k = findKey(key);
    if (k == -1)
        return nullptr;
    const ValueList& v = valuesv[k];
    if (index < 0 || index >= (int)v.size())
        return nullptr;
    return v[index].originType.c_str();
}

void cProperty::setValue(const char *key, int index, const char *value, const char *originFile, const char *originType)
{
    if (isLocked())
        throw cRuntimeError(this, E_LOCKED);
    if (!value)
        value = "";
    ValueList& v = getValuesVector(key);
    if (index < 0)
        throw cRuntimeError(this, "Negative property value index %d for key '%s'", index, key);
    if (index >= (int)v.size())
        setNumValues(key, index+1);
    v[index].value = value;
    v[index].originFile = originFile;
    v[index].originType = originType;
}

void cProperty::erase(const char *key)
{
    if (isLocked())
        throw cRuntimeError(this, E_LOCKED);

    // erase
    int k = findKey(key);
    if (k != -1) {
        keyv.erase(keyv.begin() + k);
        valuesv.erase(valuesv.begin() + k);
    }
}

void cProperty::updateWith(const cProperty *other)
{
    const std::vector<const char *>& keys = other->getKeys();
    for (auto key : keys) {
        if (!containsKey(key))
            addKey(key);
        int n = other->getNumValues(key);
        for (int index = 0; index < n; index++) {
            const char *value = other->getValue(key, index);
            if (value && value[0]) {  // is set
                bool isAntivalue = strcmp(value, "-")==0;
                if (isAntivalue)
                    setValue(key, index, "", nullptr, nullptr);
                else
                    setValue(key, index, value, other->getValueOriginFile(key, index), other->getValueOriginType(key, index));
            }
        }
    }
}

}  // namespace omnetpp

