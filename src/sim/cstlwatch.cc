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
  Copyright (C) 1992-2008 Andras Varga
  Copyright (C) 2006-2008 OpenSim Ltd.

  This file is distributed WITHOUT ANY WARRANTY. See the file
  `license' for details on this and other legal matters.
*--------------------------------------------------------------*/

#include <stdio.h>
#include "cstlwatch.h"
#include "cclassdescriptor.h"
#include "globals.h"

USING_NAMESPACE

//
// Internal
//
class SIM_API cStdVectorWatcherDescriptor : public cClassDescriptor //noncopyable
{
  public:
    cStdVectorWatcherDescriptor();
    virtual ~cStdVectorWatcherDescriptor();

    virtual const char *getProperty(const char *propertyname) const;
    virtual int getFieldCount(void *object) const;
    virtual const char *getFieldName(void *object, int field) const;
    virtual unsigned int getFieldTypeFlags(void *object, int field) const;
    virtual const char *getFieldTypeString(void *object, int field) const;
    virtual const char *getFieldProperty(void *object, int field, const char *propertyname) const;
    virtual int getArraySize(void *object, int field) const;

    virtual bool getFieldAsString(void *object, int field, int i, char *resultbuf, int bufsize) const;
    virtual bool setFieldAsString(void *object, int field, int i, const char *value) const;

    virtual const char *getFieldStructName(void *object, int field) const;
    virtual void *getFieldStructPointer(void *object, int field, int i) const;
};

Register_ClassDescriptor(cStdVectorWatcherDescriptor);

cStdVectorWatcherDescriptor::cStdVectorWatcherDescriptor() :
cClassDescriptor("cStdVectorWatcher", "cStdVectorWatcherBase")
{
}

cStdVectorWatcherDescriptor::~cStdVectorWatcherDescriptor()
{
}

const char *cStdVectorWatcherDescriptor::getProperty(const char *propertyname) const
{
    return NULL;
}

int cStdVectorWatcherDescriptor::getFieldCount(void *) const
{
    return 1;
}

unsigned int cStdVectorWatcherDescriptor::getFieldTypeFlags(void *, int field) const
{
    switch (field) {
        case 0: return FD_ISARRAY; // as far as we're concerned. We don't know about FD_ISCOMPOUND, FD_ISPOINTER, FD_ISPOLYM / FD_ISCOBJECT
        default: return FD_NONE;
    }
}

const char *cStdVectorWatcherDescriptor::getFieldName(void *object, int field) const
{
    cStdVectorWatcherBase *pp = (cStdVectorWatcherBase *)object;
    switch (field) {
        case 0: return pp->name();
        default: return NULL;
    }
}

const char *cStdVectorWatcherDescriptor::getFieldTypeString(void *object, int field) const
{
    cStdVectorWatcherBase *pp = (cStdVectorWatcherBase *)object;
    switch (field) {
        case 0: return pp->elemTypeName();
        default: return NULL;
    }
}

const char *cStdVectorWatcherDescriptor::getFieldProperty(void *object, int field, const char *propertyname) const
{
    return NULL;
}

int cStdVectorWatcherDescriptor::getArraySize(void *object, int field) const
{
    cStdVectorWatcherBase *pp = (cStdVectorWatcherBase *)object;
    switch (field) {
        case 0: return pp->size();
        default: return 0;
    }
}

bool cStdVectorWatcherDescriptor::getFieldAsString(void *object, int field, int i, char *resultbuf, int bufsize) const
{
    cStdVectorWatcherBase *pp = (cStdVectorWatcherBase *)object;
    switch (field) {
        case 0: oppstring2string(pp->at(i), resultbuf, bufsize); return true;
        default: return false;
    }
}

bool cStdVectorWatcherDescriptor::setFieldAsString(void *object, int field, int i, const char *value) const
{
    return false; // not supported
}

const char *cStdVectorWatcherDescriptor::getFieldStructName(void *object, int field) const
{
    return NULL;
}

void *cStdVectorWatcherDescriptor::getFieldStructPointer(void *object, int field, int i) const
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

cClassDescriptor *cStdVectorWatcherBase::getDescriptor()
{
    return cClassDescriptor::getDescriptorFor("cStdVectorWatcher");
}

