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
#include "omnetpp/cproperty.h"
#include "omnetpp/cproperties.h"

namespace omnetpp {

const char *cProperty::DEFAULTKEY = "";

cStringPool cProperty::stringPool("cProperty::stringPool");

cProperty::cProperty(const char *name, const char *index) : cNamedObject(name, true)
{
    ownerp = nullptr;
    propindex = propfullname = nullptr;
    setIndex(index);
}

cProperty::~cProperty()
{
    // release pooled strings
    stringPool.release(propindex);
    stringPool.release(propfullname);
    int n = keyv.size();
    for (int i = 0; i < n; i++) {
        stringPool.release(keyv[i]);
        releaseValues(valuesv[i]);
    }
}

void cProperty::releaseValues(CharPtrVector& vals)
{
    int n = vals.size();
    for (int i = 0; i < n; i++)
        stringPool.release(vals[i]);
    vals.clear();
}

void cProperty::copy(const cProperty& other)
{
    stringPool.release(propfullname);
    propfullname = nullptr;

    setIndex(other.getIndex());

    // release old value
    int n = keyv.size();
    for (int i = 0; i < n; i++) {
        stringPool.release(keyv[i]);
        releaseValues(valuesv[i]);
    }
    keyv.clear();
    valuesv.clear();

    // copy new value
    int m = other.keyv.size();
    for (int i = 0; i < m; i++) {
        keyv.push_back(stringPool.get(other.keyv[i]));
        valuesv.push_back(CharPtrVector());
        CharPtrVector& vals = valuesv[i];
        const CharPtrVector& othervals = other.valuesv[i];
        int nn = othervals.size();
        vals.resize(nn);
        for (int j = 0; j < nn; j++)
            vals[j] = stringPool.get(othervals[j]);
    }
}

cProperty& cProperty::operator=(const cProperty& other)
{
    if (this == &other)
        return *this;
    if (isLocked())
        throw cRuntimeError(this, E_LOCKED);
    cNamedObject::operator=(other);
    copy(other);
    setName(other.getName());  // cNamedObject doesn't do that
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

    stringPool.release(propfullname);
    propfullname = nullptr;
}

const char *cProperty::getFullName() const
{
    if (!propfullname) {
        if (!propindex) {
            propfullname = stringPool.get(getName());
        }
        else {
            std::stringstream os;
            os << getName() << "[" << propindex << "]";
            propfullname = stringPool.get(os.str().c_str());
        }
    }
    return propfullname;
}

std::string cProperty::str() const
{
    std::stringstream os;
    os << "@" << getFullName();
    if (!keyv.empty()) {
        os << "(";
        int n = keyv.size();
        for (int i = 0; i < n; i++) {
            if (i != 0)
                os << ";";
            if (keyv[i] && *keyv[i])
                os << keyv[i] << "=";
            for (int j = 0; j < (int)valuesv[i].size(); j++)
                os << (j == 0 ? "" : ",") << valuesv[i][j];  //FIXME value may need quoting
        }
        os << ")";
    }
    return os.str();
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

void cProperty::setIndex(const char *index)
{
    if (isLocked())
        throw cRuntimeError(this, E_LOCKED);

    stringPool.release(propindex);
    propindex = stringPool.get(index);

    stringPool.release(propfullname);
    propfullname = nullptr;
}

const char *cProperty::getIndex() const
{
    return propindex;
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
    if (!key)
        key = "";
    for (int i = 0; i < (int)keyv.size(); i++)
        if (!strcmp(key, keyv[i]))
            return i;

    return -1;
}

const std::vector<const char *>& cProperty::getKeys() const
{
    return keyv;
}

bool cProperty::containsKey(const char *key) const
{
    return findKey(key) != -1;
}

void cProperty::addKey(const char *key)
{
    if (!key)
        key = "";
    int k = findKey(key);
    if (k == -1) {
        keyv.push_back(stringPool.get(key));
        valuesv.push_back(CharPtrVector());
    }
}

cProperty::CharPtrVector& cProperty::getValuesVector(const char *key) const
{
    if (!key)
        key = "";
    int k = findKey(key);
    if (k == -1)
        throw cRuntimeError(this, "Property has no key named '%s'", key);
    return const_cast<CharPtrVector&>(valuesv[k]);
}

int cProperty::getNumValues(const char *key) const
{
    if (!key)
        key = "";
    int k = findKey(key);
    if (k == -1)
        return 0;
    return valuesv[k].size();
}

void cProperty::setNumValues(const char *key, int size)
{
    if (isLocked())
        throw cRuntimeError(this, E_LOCKED);
    CharPtrVector& v = getValuesVector(key);
    int oldsize = v.size();

    // if shrink, release extra elements
    for (int i = size; i < oldsize; i++)
        stringPool.release(v[i]);

    // resize
    v.resize(size);

    // if grow, initialize extra elements
    for (int i = oldsize; i < size; i++)
        v[i] = stringPool.get("");
}

const char *cProperty::getValue(const char *key, int index) const
{
    if (!key)
        key = "";
    int k = findKey(key);
    if (k == -1)
        return nullptr;
    const CharPtrVector& v = valuesv[k];
    if (index < 0 || index >= (int)v.size())
        return nullptr;
    return v[index];
}

void cProperty::setValue(const char *key, int index, const char *value)
{
    if (isLocked())
        throw cRuntimeError(this, E_LOCKED);
    if (!value)
        value = "";
    CharPtrVector& v = getValuesVector(key);
    if (index < 0)
        throw cRuntimeError(this, "Negative property value index %d for key '%s'", index, key);
    if (index >= (int)v.size())
        setNumValues(key, index+1);
    stringPool.release(v[index]);
    v[index] = stringPool.get(value);
}

void cProperty::erase(const char *key)
{
    if (isLocked())
        throw cRuntimeError(this, E_LOCKED);

    // erase
    int k = findKey(key);
    if (k != -1) {
        stringPool.release(keyv[k]);
        releaseValues(valuesv[k]);
        keyv.erase(keyv.begin() + k);
        valuesv.erase(valuesv.begin() + k);
    }
}

void cProperty::updateWith(const cProperty *property)
{
    const std::vector<const char *>& keys = property->getKeys();
    for (auto key : keys) {
        if (!containsKey(key))
            addKey(key);
        int n = property->getNumValues(key);
        for (int index = 0; index < n; index++) {
            const char *value = property->getValue(key, index);
            if (value && value[0]) {  // is set
                bool isAntivalue = strcmp(value, "-")==0;
                setValue(key, index, isAntivalue ? "" : value);
            }
        }
    }
}

}  // namespace omnetpp

