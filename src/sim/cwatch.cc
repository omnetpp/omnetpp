//=========================================================================
//  CWATCH.CC - part of
//
//                  OMNeT++/OMNEST
//           Discrete System Simulation in C++
//
//
//   Member functions of
//    cWatchBase etc: make primitive types, structs etc inspectable
//
//  Author: Andras Varga
//
//=========================================================================

/*--------------------------------------------------------------*
  Copyright (C) 1992-2017 Andras Varga
  Copyright (C) 2006-2017 OpenSim Ltd.

  This file is distributed WITHOUT ANY WARRANTY. See the file
  `license' for details on this and other legal matters.
*--------------------------------------------------------------*/

#include "common/stringutil.h"
#include "omnetpp/cwatch.h"
#include "omnetpp/globals.h"
#include "omnetpp/cclassdescriptor.h"

using namespace omnetpp::common;

namespace omnetpp {

/**
 * Internal helper for cWatchBase (for objects and object pointers).
 * Necessary only to translate the object argument of instance-related
 * methods from the cWatchBase (or subclass) pointer to a pointer to
 * the actually watched object.
 */
class cWatchProxyDescriptor : public cClassDescriptor {
    cClassDescriptor *watchedDescriptor; // most methods delegate to this

  protected:

    // The inspectors will inspect a cWatchBase (or subclass) instance, but the
    // descriptor of the watched object expects a pointer to its own (described) type.
    // This is a helper that turns the former into the latter.
    static cObject *castToWatchAndGetWatchedObject(void *object) {
        cObject *cObj = (cObject *)object;
        if (auto watch_obj = dynamic_cast<cWatch_cObject*>(cObj))
            return &watch_obj->r;
        if (auto watch_ptr = dynamic_cast<cWatch_cObjectPtr*>(cObj))
            return watch_ptr->rp;
        return nullptr;
    }

  public:
    cWatchProxyDescriptor(cClassDescriptor *watchedDescriptor)
      : cClassDescriptor(watchedDescriptor->getClassName()), watchedDescriptor(watchedDescriptor)
    { }

    virtual const char **getPropertyNames() const override { return watchedDescriptor->getPropertyNames(); }
    virtual const char *getProperty(const char *propertyname) const override { return watchedDescriptor->getProperty(propertyname); }

    virtual int getFieldCount() const override { return watchedDescriptor->getFieldCount(); }
    virtual const char *getFieldName(int field) const override { return watchedDescriptor->getFieldName(field); }
    virtual unsigned int getFieldTypeFlags(int field) const override { return watchedDescriptor->getFieldTypeFlags(field); }
    virtual const char *getFieldTypeString(int field) const override { return watchedDescriptor->getFieldTypeString(field); }
    virtual const char *getFieldStructName(int field) const override { return watchedDescriptor->getFieldStructName(field); }

    virtual const char **getFieldPropertyNames(int field) const override { return watchedDescriptor->getFieldPropertyNames(field); }
    virtual const char *getFieldProperty(int field, const char *propertyname) const override { return watchedDescriptor->getFieldProperty(field, propertyname); }

    virtual std::string getFieldValueAsString(void *object, int field, int i) const override {
        cObject *watched = castToWatchAndGetWatchedObject(object);
        return watchedDescriptor->getFieldValueAsString(watched, field, i);
    }

    virtual void setFieldValueAsString(void *object, int field, int i, const char *value) const override {
        cObject *watched = castToWatchAndGetWatchedObject(object);
        watchedDescriptor->setFieldValueAsString(watched, field, i, value);
    }

    virtual void *getFieldStructValuePointer(void *object, int field, int i) const override {
        cObject *watched = castToWatchAndGetWatchedObject(object);
        return watchedDescriptor->getFieldStructValuePointer(watched, field, i);
    }

    virtual void setFieldStructValuePointer(void *object, int field, int i, void *ptr) const override {
        cObject *watched = castToWatchAndGetWatchedObject(object);
        watchedDescriptor->setFieldStructValuePointer(watched, field, i, ptr);
    }

    virtual int getFieldArraySize(void *object, int field) const override {
        cObject *watched = castToWatchAndGetWatchedObject(object);
        return watchedDescriptor->getFieldArraySize(watched, field);
    }

    virtual void setFieldArraySize(void *object, int field, int size) const override {
        cObject *watched = castToWatchAndGetWatchedObject(object);
        watchedDescriptor->setFieldArraySize(watched, field, size);
    }
};

// ----

cWatch_cObject::cWatch_cObject(const char *name, cObject& ref)
    : cWatchBase(name), r(ref)
{
    // lookup the proxy descriptor, or create and register it if it wasn't yet
    desc = (cClassDescriptor *)classDescriptors.getInstance()->lookup("cWatch_cObject");
    if (!desc) {
        desc = new cWatchProxyDescriptor(r.getDescriptor());
        classDescriptors.getInstance()->add(desc);
    }
}

cWatch_cObjectPtr::cWatch_cObjectPtr(const char *name, cObject *&ptr)
    : cWatchBase(name), rp(ptr)
{
    // lookup the proxy descriptor, or create and register it if it wasn't yet
    desc = (cClassDescriptor *)classDescriptors.getInstance()->lookup("cWatch_cObjectPtr");
    if (!desc) {
        desc = new cWatchProxyDescriptor(rp->getDescriptor());
        classDescriptors.getInstance()->add(desc);
    }
}

// ----

std::string cWatch_stdstring::str() const
{
    return opp_quotestr(r);
}

void cWatch_stdstring::assign(const char *s)
{
    if (s[0] == '"' && s[strlen(s)-1] == '"') {
        r = opp_parsequotedstr(s);
    }
    else {
        r = s;
    }
}

}  // namespace omnetpp

