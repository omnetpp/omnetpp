//==========================================================================
//  CSTLWATCH.H - part of
//                     OMNeT++/OMNEST
//            Discrete System Simulation in C++
//
//
//  WATCH_VECTOR, WATCH_MAP etc macros
//
//==========================================================================

/*--------------------------------------------------------------*
  Copyright (C) 1992-2005 Andras Varga

  This file is distributed WITHOUT ANY WARRANTY. See the file
  `license' for details on this and other legal matters.
*--------------------------------------------------------------*/

#include <stdio.h>
#include "cstlwatch.h"
#include "cstruct.h"
#include "macros.h"

//
// Internal
//
class SIM_API cVectorWatchDescriptor : public cStructDescriptor
{
  public:
    cVectorWatchDescriptor();
    virtual ~cVectorWatchDescriptor();
    cVectorWatchDescriptor& operator=(const cVectorWatchDescriptor& other);
    virtual cObject *dup() const {return new cVectorWatchDescriptor(*this);}

    virtual int getFieldCount();
    virtual const char *getFieldName(int field);
    virtual int getFieldType(int field);
    virtual const char *getFieldTypeString(int field);
    virtual const char *getFieldEnumName(int field);
    virtual int getArraySize(int field);

    virtual bool getFieldAsString(int field, int i, char *resultbuf, int bufsize);
    virtual bool setFieldAsString(int field, int i, const char *value);

    virtual const char *getFieldStructName(int field);
    virtual void *getFieldStructPointer(int field, int i);
    virtual sFieldWrapper *getFieldWrapper(int field, int i);
};

Register_Class(cVectorWatchDescriptor);

cVectorWatchDescriptor::cVectorWatchDescriptor() : cStructDescriptor("cObject")
{
}

cVectorWatchDescriptor::~cVectorWatchDescriptor()
{
}

int cVectorWatchDescriptor::getFieldCount()
{
    return 1;
}

int cVectorWatchDescriptor::getFieldType(int field)
{
    switch (field) {
        case 0: return FT_BASIC_ARRAY;
        default: return FT_INVALID;
    }
}

const char *cVectorWatchDescriptor::getFieldName(int field)
{
    cVectorWatcherBase *pp = (cVectorWatcherBase *)p;
    switch (field) {
        case 0: return pp->name();
        default: return NULL;
    }
}

const char *cVectorWatchDescriptor::getFieldTypeString(int field)
{
    cVectorWatcherBase *pp = (cVectorWatcherBase *)p;
    switch (field) {
        case 0: return pp->elemTypeName();
        default: return NULL;
    }
}

const char *cVectorWatchDescriptor::getFieldEnumName(int field)
{
    switch (field) {
        default: return NULL;
    }
}

int cVectorWatchDescriptor::getArraySize(int field)
{
    cVectorWatcherBase *pp = (cVectorWatcherBase *)p;
    switch (field) {
        case 0: return pp->size();
        default: return 0;
    }
}

bool cVectorWatchDescriptor::getFieldAsString(int field, int i, char *resultbuf, int bufsize)
{
    cVectorWatcherBase *pp = (cVectorWatcherBase *)p;
    switch (field) {
        case 0: oppstring2string(pp->at(i).c_str(),resultbuf,bufsize); return true;
        default: return false;
    }
}

bool cVectorWatchDescriptor::setFieldAsString(int field, int i, const char *value)
{
    return false; // not supported
}

const char *cVectorWatchDescriptor::getFieldStructName(int field)
{
    return NULL;
}

void *cVectorWatchDescriptor::getFieldStructPointer(int field, int i)
{
    return NULL;
}

sFieldWrapper *cVectorWatchDescriptor::getFieldWrapper(int field, int i)
{
    return NULL;
}

//--------------------------------

std::string cVectorWatcherBase::info() const
{
    if (size()==0)
        return std::string("empty");
    std::stringstream out;
    out << "size=" << size();
    return out.str();
}

std::string cVectorWatcherBase::detailedInfo() const
{
    std::stringstream out;
    int n = size()<=3 ? size() : 3;
    for (int i=0; i<n; i++)
        out << fullName() << "[" << i << "] = " << at(i) << "\n";
    if (size()>3)
        out << "...\n";
    return out.str();
}

cStructDescriptor *cVectorWatcherBase::createDescriptor()
{
    cStructDescriptor *sd = new cVectorWatchDescriptor();
    sd->setStruct(this);
    return sd;
}

