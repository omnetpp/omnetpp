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

USING_NAMESPACE


const char *cProperty::DEFAULTKEY = "";

cStringPool cProperty::stringPool;


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

cProperty& cProperty::operator=(const cProperty& other)
{
    if (isLocked())
        throw cRuntimeError(this, eLOCKED);

    // note: do NOT copy islocked flag
    bool oldIsLocked = isLocked();
    cNamedObject::operator=(other);
    setFlag(FL_ISLOCKED, oldIsLocked);

    setName(other.name()); // cNamedObject doesn't do that

    stringPool.release(propfullname);
    propfullname = NULL;

    setIndex(other.index());

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

const char *cProperty::fullName() const
{
    if (!propfullname)
    {
        if (!propindex) {
            propfullname = stringPool.get(name());
        }
        else {
            std::stringstream os;
            os << name() << "[" << propindex << "]";
            propfullname = stringPool.get(os.str().c_str());
        }
    }
    return propfullname;
}

std::string cProperty::info() const
{
    std::stringstream os;
    os << "@" << fullName();
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

void cProperty::netPack(cCommBuffer *buffer)
{
    throw cRuntimeError(this, eCANTPACK);
}

void cProperty::netUnpack(cCommBuffer *buffer)
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

const char *cProperty::index() const
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

const std::vector<const char *>& cProperty::keys() const
{
    return keyv;
}

bool cProperty::hasKey(const char *key) const
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

cProperty::CharPtrVector& cProperty::valuesVector(const char *key) const
{
    if (!key)
        key = "";
    int k = findKey(key);
    if (k==-1)
        throw cRuntimeError(this, "property has no key named `%s'", key);
    return const_cast<CharPtrVector&>(valuesv[k]);
}

int cProperty::numValues(const char *key) const
{
    CharPtrVector& v = valuesVector(key);
    return v.size();
}

void cProperty::setNumValues(const char *key, int size)
{
    if (isLocked())
        throw cRuntimeError(this, eLOCKED);
    CharPtrVector& v = valuesVector(key);
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

const char *cProperty::value(const char *key, int k) const
{
    CharPtrVector& v = valuesVector(key);
    if (k<0 || k>=(int)v.size())
        return "";
    return v[k];
}

void cProperty::setValue(const char *key, int k, const char *value)
{
    if (isLocked())
        throw cRuntimeError(this, eLOCKED);
    if (!value)
        value = "";
    CharPtrVector& v = valuesVector(key);
    if (k<0)
        throw cRuntimeError(this, "negative property value index %d for key `%s'", k, key);
    if (k>=(int)v.size())
        setNumValues(key, k+1);
    stringPool.release(v[k]);
    v[k] = stringPool.get(value);
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

