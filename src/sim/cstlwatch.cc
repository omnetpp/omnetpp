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
#include "cclassdescriptor.h"
#include "macros.h"

//
// Internal
//
class SIM_API cStdVectorWatcherDescriptor : public cClassDescriptor
{
  public:
    cStdVectorWatcherDescriptor();
    virtual ~cStdVectorWatcherDescriptor();
    cStdVectorWatcherDescriptor& operator=(const cStdVectorWatcherDescriptor& other) {copyNotSupported(); return *this;}
    virtual cPolymorphic *dup() const {return new cStdVectorWatcherDescriptor(*this);}

    virtual const char *getProperty(const char *propertyname);
    virtual int getFieldCount(void *object);
    virtual const char *getFieldName(void *object, int field);
    virtual unsigned int getFieldTypeFlags(void *object, int field);
    virtual const char *getFieldTypeString(void *object, int field);
    virtual const char *getFieldProperty(void *object, int field, const char *propertyname);
    virtual int getArraySize(void *object, int field);

    virtual bool getFieldAsString(void *object, int field, int i, char *resultbuf, int bufsize);
    virtual bool setFieldAsString(void *object, int field, int i, const char *value);

    virtual const char *getFieldStructName(void *object, int field);
    virtual void *getFieldStructPointer(void *object, int field, int i);
};

Register_ClassDescriptor(cStdVectorWatcherDescriptor);

cStdVectorWatcherDescriptor::cStdVectorWatcherDescriptor() :
cClassDescriptor("cStdVectorWatcher", "cStdVectorWatcherBase")
{
}

cStdVectorWatcherDescriptor::~cStdVectorWatcherDescriptor()
{
}

const char *cStdVectorWatcherDescriptor::getProperty(const char *propertyname)
{
    return NULL;
}

int cStdVectorWatcherDescriptor::getFieldCount(void *)
{
    return 1;
}

unsigned int cStdVectorWatcherDescriptor::getFieldTypeFlags(void *, int field)
{
    switch (field) {
        case 0: return FD_ISARRAY; // as far as we're concerned. We don't know about FD_ISCOMPOUND, FD_ISPOINTER, FD_ISPOLYM / FD_ISCOBJECT
        default: return FD_NONE;
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

const char *cStdVectorWatcherDescriptor::getFieldProperty(void *object, int field, const char *propertyname)
{
    return NULL;
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

cClassDescriptor *cStdVectorWatcherBase::getDescriptor()
{
    return cClassDescriptor::getDescriptorFor("cStdVectorWatcher");
}

