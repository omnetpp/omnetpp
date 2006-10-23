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
#include "globals.h"

//
// Internal
//
class SIM_API cStdVectorWatcherDescriptor : public cStructDescriptor //noncopyable
{
  public:
    cStdVectorWatcherDescriptor();
    virtual ~cStdVectorWatcherDescriptor();

    virtual int getFieldCount(void *object);
    virtual const char *getFieldName(void *object, int field);
    virtual int getFieldType(void *object, int field);
    virtual const char *getFieldTypeString(void *object, int field);
    virtual const char *getFieldEnumName(void *object, int field);
    virtual int getArraySize(void *object, int field);

    virtual bool getFieldAsString(void *object, int field, int i, char *resultbuf, int bufsize);
    virtual bool setFieldAsString(void *object, int field, int i, const char *value);

    virtual const char *getFieldStructName(void *object, int field);
    virtual void *getFieldStructPointer(void *object, int field, int i);
};

Register_ClassDescriptor(cStdVectorWatcherDescriptor);

cStdVectorWatcherDescriptor::cStdVectorWatcherDescriptor() :
cStructDescriptor("cStdVectorWatcher", "cStdVectorWatcherBase")
{
}

cStdVectorWatcherDescriptor::~cStdVectorWatcherDescriptor()
{
}

int cStdVectorWatcherDescriptor::getFieldCount(void *)
{
    return 1;
}

int cStdVectorWatcherDescriptor::getFieldType(void *, int field)
{
    switch (field) {
        case 0: return FT_BASIC_ARRAY;
        default: return FT_INVALID;
    }
}

const char *cStdVectorWatcherDescriptor::getFieldName(void *object, int field)
{
    cStdVectorWatcherBase *pp = (cStdVectorWatcherBase *)object;
    switch (field) {
        case 0: return pp->name();
        default: return NULL;
    }
}

const char *cStdVectorWatcherDescriptor::getFieldTypeString(void *object, int field)
{
    cStdVectorWatcherBase *pp = (cStdVectorWatcherBase *)object;
    switch (field) {
        case 0: return pp->elemTypeName();
        default: return NULL;
    }
}

const char *cStdVectorWatcherDescriptor::getFieldEnumName(void *object, int field)
{
    switch (field) {
        default: return NULL;
    }
}

int cStdVectorWatcherDescriptor::getArraySize(void *object, int field)
{
    cStdVectorWatcherBase *pp = (cStdVectorWatcherBase *)object;
    switch (field) {
        case 0: return pp->size();
        default: return 0;
    }
}

bool cStdVectorWatcherDescriptor::getFieldAsString(void *object, int field, int i, char *resultbuf, int bufsize)
{
    cStdVectorWatcherBase *pp = (cStdVectorWatcherBase *)object;
    switch (field) {
        case 0: oppstring2string(pp->at(i), resultbuf, bufsize); return true;
        default: return false;
    }
}

bool cStdVectorWatcherDescriptor::setFieldAsString(void *object, int field, int i, const char *value)
{
    return false; // not supported
}

const char *cStdVectorWatcherDescriptor::getFieldStructName(void *object, int field)
{
    return NULL;
}

void *cStdVectorWatcherDescriptor::getFieldStructPointer(void *object, int field, int i)
{
    return NULL;
}


//--------------------------------

std::string cStdVectorWatcherBase::info() const
{
    if (size()==0)
        return std::string("empty");
    std::stringstream out;
    out << "size=" << size();
    return out.str();
}

std::string cStdVectorWatcherBase::detailedInfo() const
{
    std::stringstream out;
    int n = size()<=3 ? size() : 3;
    for (int i=0; i<n; i++)
        out << fullName() << "[" << i << "] = " << at(i) << "\n";
    if (size()>3)
        out << "...\n";
    return out.str();
}

cStructDescriptor *cStdVectorWatcherBase::getDescriptor()
{
    return cStructDescriptor::getDescriptorFor("cStdVectorWatcher");
}

