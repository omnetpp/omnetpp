//==========================================================================
//  CPROPERTY.CC - part of
//                     OMNeT++/OMNEST
//            Discrete System Simulation in C++
//
//==========================================================================

/*--------------------------------------------------------------*
  Copyright (C) 1992-2005 Andras Varga

  This file is distributed WITHOUT ANY WARRANTY. See the file
  `license' for details on this and other legal matters.
*--------------------------------------------------------------*/

#include "cproperty.h"
#include "cproperties.h"


cProperty::cProperty(const char *name)
{
    isimplicit = islocked = false;
    ownerp = NULL;
    propertyname = NULL;
    if (name)
        setName(name);
}

cProperty::~cProperty()
{
    // release pooled strings
    stringPool.release(propertyname);
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

cProperty& cProperty::operator=(const cProperty& other)
{
    if (islocked)
        throw new cRuntimeError(this, eLOCKED);

    // note: do NOT copy islocked flag
    setName(other.fullName());

    // release old value
    stringPool.release(propertyname);
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
    return *this;
}

void cProperty::setName(const char *name)
{
    if (islocked)
        throw new cRuntimeError(this, eLOCKED);
    if (!name || name[0]!='@')
        throw new cRuntimeError(this,"setName(): property name must begin with '@' character");
    if (!propertyname)
        stringPool.release(propertyname);
    propertyname = stringPool.get(name);
}

const char *cProperty::fullName() const
{
    return propertyname;
}

std::string cProperty::fullPath() const
{
    // FIXME what to do if it's a shared property and owner is not set?
    return ownerp ? ownerp->fullPath()+"."+fullName() : fullName(); //FIXME not good! plus use sstream
}

std::string cProperty::info() const
{
    return "";  //TBD
}

void cProperty::netPack(cCommBuffer *buffer)
{
    //TBD
}

void cProperty::netUnpack(cCommBuffer *buffer)
{
    if (islocked)
        throw new cRuntimeError(this, eLOCKED);
    //TBD
}

void cProperty::setIsImplicit(bool b)
{
    if (islocked)
        throw new cRuntimeError(this, eLOCKED);
    isimplicit = b;
}

bool cProperty::isImplicit() const
{
    return isimplicit;
}

void cProperty::setIndex(short index)
{
    if (islocked)
        throw new cRuntimeError(this, eLOCKED);
    index_ = index;
}

short cProperty::index() const
{
    return index_;
}

int cProperty::findKey(const char *key) const
{
    for (int i=0; i<keyv.size(); i++)
        if (!strcmp(key,keyv[i]))
            return i;
    return -1;
}

const std::vector<const char *>& cProperty::keys() const
{
    return keyv;
}

bool cProperty::hasKey(const char *key) const
{
    return findKey(key)!=-1;
}

const std::vector<const char *>& cProperty::values(const char *key) const
{
    int k = findKey(key);
    if (k==-1)
        throw cRuntimeError(this,"values(): property has no key named `%s'", key);
    return valuesv[k];
}

void cProperty::setValues(const char *key, const std::vector<const char *>& v)
{
    if (islocked)
        throw new cRuntimeError(this, eLOCKED);

    int k = findKey(key);
    if (k==-1)
    {
        k = keyv.size();
        keyv.push_back(stringPool.get(key));
        valuesv.push_back(CharPtrVector());
    }
    CharPtrVector& vals = valuesv[k];
    releaseValues(vals);
    int n = v.size();
    vals.resize(n);
    for (int i=0; i<n; i++)
        vals[i] = stringPool.get(v[i]);
}

void cProperty::erase(const char *key)
{
    if (islocked)
        throw new cRuntimeError(this, eLOCKED);

    // erase
    int k = findKey(key);
    if (k==-1)
    {
        stringPool.release(keyv[k]);
        releaseValues(valuesv[k]);
        keyv.erase(keyv.begin()+k);
        valuesv.erase(valuesv.begin()+k);
    }
}

