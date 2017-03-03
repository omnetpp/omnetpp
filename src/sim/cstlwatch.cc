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
  Copyright (C) 1992-2017 Andras Varga
  Copyright (C) 2006-2017 OpenSim Ltd.

  This file is distributed WITHOUT ANY WARRANTY. See the file
  `license' for details on this and other legal matters.
*--------------------------------------------------------------*/

#include <cstdio>
#include "omnetpp/cstlwatch.h"
#include "omnetpp/cclassdescriptor.h"
#include "omnetpp/globals.h"

namespace omnetpp {

//
// Internal
//
class SIM_API cStdVectorWatcherDescriptor : public cClassDescriptor  // noncopyable
{
  private:
    std::string vectorTypeName;  // type name of the inspected type, e.g. "std::vector<foo::Bar>"
    std::string elementTypeName;  // type name of vector elements, e.g. "foo::Bar"

  public:
    cStdVectorWatcherDescriptor(const char *vecTypeName, const char *elemTypeName);
    virtual ~cStdVectorWatcherDescriptor();

    virtual const char **getPropertyNames() const override;
    virtual const char *getProperty(const char *propertyname) const override;
    virtual int getFieldCount() const override;
    virtual const char *getFieldName(int field) const override;
    virtual unsigned int getFieldTypeFlags(int field) const override;
    virtual const char *getFieldTypeString(int field) const override;
    virtual const char **getFieldPropertyNames(int field) const override;
    virtual const char *getFieldProperty(int field, const char *propertyname) const override;
    virtual int getFieldArraySize(void *object, int field) const override;

    virtual std::string getFieldValueAsString(void *object, int field, int i) const override;
    virtual bool setFieldValueAsString(void *object, int field, int i, const char *value) const override;

    virtual const char *getFieldStructName(int field) const override;
    virtual void *getFieldStructValuePointer(void *object, int field, int i) const override;
};

cStdVectorWatcherDescriptor::cStdVectorWatcherDescriptor(const char *vecType, const char *elemType) :
    cClassDescriptor(vecType, nullptr)
{
    vectorTypeName = vecType;
    elementTypeName = elemType;
}

cStdVectorWatcherDescriptor::~cStdVectorWatcherDescriptor()
{
}

const char **cStdVectorWatcherDescriptor::getPropertyNames() const
{
    static const char **names = {
        nullptr
    };
    return names;
}

const char *cStdVectorWatcherDescriptor::getProperty(const char *propertyname) const
{
    return nullptr;
}

int cStdVectorWatcherDescriptor::getFieldCount() const
{
    return 1;
}

unsigned int cStdVectorWatcherDescriptor::getFieldTypeFlags(int field) const
{
    return FD_ISARRAY;  //TODO we could return FD_ISCOMPOUND, FD_ISPOINTER, FD_ISCOBJECT / FD_ISCOWNEDOBJECT, with a little help from cStdVectorWatcherBase, and then elements would become inspectable (currently they displayed as just strings)
}

const char *cStdVectorWatcherDescriptor::getFieldName(int field) const
{
    return "elements";
}

const char *cStdVectorWatcherDescriptor::getFieldTypeString(int field) const
{
    return elementTypeName.c_str();
}

const char **cStdVectorWatcherDescriptor::getFieldPropertyNames(int field) const
{
    static const char **names = { nullptr };
    return names;
}

const char *cStdVectorWatcherDescriptor::getFieldProperty(int field, const char *propertyname) const
{
    return nullptr;
}

int cStdVectorWatcherDescriptor::getFieldArraySize(void *object, int field) const
{
    cStdVectorWatcherBase *pp = (cStdVectorWatcherBase *)object;
    return pp->size();
}

std::string cStdVectorWatcherDescriptor::getFieldValueAsString(void *object, int field, int i) const
{
    cStdVectorWatcherBase *pp = (cStdVectorWatcherBase *)object;
    return pp->at(i);
}

bool cStdVectorWatcherDescriptor::setFieldValueAsString(void *object, int field, int i, const char *value) const
{
    return false;  // not supported
}

const char *cStdVectorWatcherDescriptor::getFieldStructName(int field) const
{
    return nullptr;  //TODO we could return elementTypeName (if it is a compound type; if it's a pointer, the '*' should be removed)
}

void *cStdVectorWatcherDescriptor::getFieldStructValuePointer(void *object, int field, int i) const
{
    return nullptr;  //TODO we could return a pointer to the given array element (if element is a compound type)
}

//--------------------------------

std::string cStdVectorWatcherBase::str() const
{
    if (size() == 0)
        return std::string("empty");
    std::stringstream out;
    out << "size=" << size();
    return out.str();
}

cClassDescriptor *cStdVectorWatcherBase::getDescriptor() const
{
    if (!desc) {
        // try to find existing descriptor for this particular type (e.g. "std::vector<double>");
        // if there isn't, create and register a new one
        desc = (cClassDescriptor *)classDescriptors.getInstance()->lookup(getClassName());
        if (!desc) {
            desc = new cStdVectorWatcherDescriptor(getClassName(), getElemTypeName());
            classDescriptors.getInstance()->add(desc);
        }
    }
    return desc;
}

}  // namespace omnetpp

