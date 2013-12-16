//==========================================================================
//  CPROPERTY.CC - part of
//                     OMNeT++/OMNEST
//            Discrete System Simulation in C++
//
//==========================================================================

/*--------------------------------------------------------------*
  Copyright (C) 1992-2008 Andras Varga
  Copyright (C) 2006-2008 OpenSim Ltd.

  This file is distributed WITHOUT ANY WARRANTY. See the file
  `license' for details on this and other legal matters.
*--------------------------------------------------------------*/

#include <sstream>
#include "cproperty.h"
#include "cproperties.h"

NAMESPACE_BEGIN


const char *cProperty::DEFAULTKEY = "";

cStringPool cProperty::stringPool("cProperty::stringPool");


cProperty::cProperty(const char *name, const char *index) : cNamedObject(name, true)
{
    ownerp = NULL;
    propindex = propfullname = NULL;
    setIndex(index);
}

cProperty::~cProperty()
{
    // release pooled strings
    stringPool.release(propindex);
    stringPool.release(propfullname);
    int n = keyv.size();
    for (int i=0; i<n; i++)
    {
        stringPool.release(keyv[i]);
        releaseValues(valuesv[i]);
    }
}

void cProperty::releaseValues(CharPtrVector& vals)
{
    int n = vals.size();
    for (int i=0; i<n; i++)
        stringPool.release(vals[i]);
    vals.clear();
}

void cProperty::copy(const cProperty& other)
{
    stringPool.release(propfullname);
    propfullname = NULL;

    setIndex(other.getIndex());

    // release old value
    int n = keyv.size();
    for (int i=0; i<n; i++)
    {
        stringPool.release(keyv[i]);
        releaseValues(valuesv[i]);
    }
    keyv.clear();
    valuesv.clear();

    // copy new value
    int m = other.keyv.size();
    for (int i=0; i<m; i++)
    {
        keyv.push_back(stringPool.get(other.keyv[i]));
        valuesv.push_back(CharPtrVector());
        CharPtrVector& vals = valuesv[i];
        const CharPtrVector& othervals = other.valuesv[i];
        int nn = othervals.size();
        vals.resize(nn);
        for (int j=0; j<nn; j++)
            vals[j] = stringPool.get(othervals[j]);
    }
}

cProperty& cProperty::operator=(const cProperty& other)
{
    if (this==&other) return *this;
    if (isLocked())
        throw cRuntimeError(this, eLOCKED);
    cNamedObject::operator=(other);
    copy(other);
    setName(other.getName()); // cNamedObject doesn't do that
    setFlag(FL_ISLOCKED, false);
    return *this;
}

void cProperty::setName(const char *name)
{
    if (isLocked())
        throw cRuntimeError(this, eLOCKED);
    if (name && name[0]=='@')
        throw cRuntimeError(this,"setName(): property name must be specified without the '@' character");

    cNamedObject::setName(name);

    stringPool.release(propfullname);
    propfullname = NULL;
}

const char *cProperty::getFullName() const
{
    if (!propfullname)
    {
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

std::string cProperty::info() const
{
    std::stringstream os;
    os << "@" << getFullName();
    if (!keyv.empty())
    {
        os << "(";
        int n = keyv.size();
        for (int i=0; i<n; i++)
        {
            if (i!=0)
                os << ";";
            if (keyv[i] && *keyv[i])
                os << keyv[i] << "=";
            for (int j=0; j<(int)valuesv[i].size(); j++)
                os << (j==0 ? "" : ",") << valuesv[i][j];  //FIXME value may need quoting
        }
        os << ")";
    }
    return os.str();
}

void cProperty::parsimPack(cCommBuffer *buffer)
{
    throw cRuntimeError(this, eCANTPACK);
}

void cProperty::parsimUnpack(cCommBuffer *buffer)
{
    if (isLocked())
        throw cRuntimeError(this, eLOCKED);
    throw cRuntimeError(this, eCANTPACK);
}

void cProperty::setIndex(const char *index)
{
    if (isLocked())
        throw cRuntimeError(this, eLOCKED);

    stringPool.release(propindex);
    propindex = stringPool.get(index);

    stringPool.release(propfullname);
    propfullname = NULL;
}

const char *cProperty::getIndex() const
{
    return propindex;
}

void cProperty::setIsImplicit(bool b)
{
    if (isLocked())
        throw cRuntimeError(this, eLOCKED);
    setFlag(FL_ISIMPLICIT, b);
}

bool cProperty::isImplicit() const
{
    return flags&FL_ISIMPLICIT;
}

int cProperty::findKey(const char *key) const
{
    if (!key)
        key = "";
    for (int i=0; i<(int)keyv.size(); i++)
        if (!strcmp(key,keyv[i]))
            return i;
    return -1;
}

const std::vector<const char *>& cProperty::getKeys() const
{
    return keyv;
}

bool cProperty::containsKey(const char *key) const
{
    return findKey(key)!=-1;
}

void cProperty::addKey(const char *key)
{
    if (!key)
        key = "";
    int k = findKey(key);
    if (k==-1)
    {
        keyv.push_back(stringPool.get(key));
        valuesv.push_back(CharPtrVector());
    }
}

cProperty::CharPtrVector& cProperty::getValuesVector(const char *key) const
{
    if (!key)
        key = "";
    int k = findKey(key);
    if (k==-1)
        throw cRuntimeError(this, "property has no key named `%s'", key);
    return const_cast<CharPtrVector&>(valuesv[k]);
}

int cProperty::getNumValues(const char *key) const
{
    if (!key)
        key = "";
    int k = findKey(key);
    if (k==-1)
        return 0;
    return valuesv[k].size();
}

void cProperty::setNumValues(const char *key, int size)
{
    if (isLocked())
        throw cRuntimeError(this, eLOCKED);
    CharPtrVector& v = getValuesVector(key);
    int oldsize = v.size();

    // if shrink, release extra elements
    for (int i=size; i<oldsize; i++)
        stringPool.release(v[i]);

    // resize
    v.resize(size);

    // if grow, initialize extra elements
    for (int i=oldsize; i<size; i++)
        v[i] = stringPool.get("");
}

const char *cProperty::getValue(const char *key, int index) const
{
    if (!key)
        key = "";
    int k = findKey(key);
    if (k==-1)
        return NULL;
    const CharPtrVector& v = valuesv[k];
    if (index<0 || index>=(int)v.size())
        return NULL;
    return v[index];
}

void cProperty::setValue(const char *key, int index, const char *value)
{
    if (isLocked())
        throw cRuntimeError(this, eLOCKED);
    if (!value)
        value = "";
    CharPtrVector& v = getValuesVector(key);
    if (index<0)
        throw cRuntimeError(this, "negative property value index %d for key `%s'", index, key);
    if (index>=(int)v.size())
        setNumValues(key, index+1);
    stringPool.release(v[index]);
    v[index] = stringPool.get(value);
}

void cProperty::erase(const char *key)
{
    if (isLocked())
        throw cRuntimeError(this, eLOCKED);

    // erase
    int k = findKey(key);
    if (k!=-1)
    {
        stringPool.release(keyv[k]);
        releaseValues(valuesv[k]);
        keyv.erase(keyv.begin()+k);
        valuesv.erase(valuesv.begin()+k);
    }
}

NAMESPACE_END

