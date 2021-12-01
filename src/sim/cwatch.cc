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
#include "omnetpp/cvalue.h"

using namespace omnetpp::common;
using namespace omnetpp::internal;

namespace omnetpp {

/**
 * Internal helper for cWatchBase (for objects and object pointers).
 * Necessary only to translate the object argument of instance-related
 * methods from the cWatchBase (or subclass) pointer to a pointer to
 * the actually watched object.
 */
class cWatchProxyDescriptor : public cClassDescriptor {
    cWatchBase *watchToDescribe;

  protected:
    // The inspectors will inspect a cWatchBase (or subclass) instance, but the
    // descriptor of the watched object expects a pointer to its own (described) type.
    // This is a helper that turns the former into the latter.
    cObject *getWatchedObject() const {
        cObject *watched = nullptr;
        if (auto watch_obj = dynamic_cast<cWatch_cObject*>(watchToDescribe))
            watched = &watch_obj->r;
        else if (auto watch_ptr = dynamic_cast<cWatch_cObjectPtr*>(watchToDescribe))
            watched = watch_ptr->rp;
        return watched;
    }

    cClassDescriptor *getWatchedDescriptor() const {
        cObject *watched = getWatchedObject();
        return watched ? watched->getDescriptor() : cClassDescriptor::getDescriptorFor("omnetpp::cObject");
    }

  public:
    cWatchProxyDescriptor(cWatchBase *watchToDescribe) : cClassDescriptor("cWatchBase"), watchToDescribe(watchToDescribe) { }
    virtual const char **getPropertyNames() const override { return getWatchedDescriptor()->getPropertyNames(); }
    virtual const char *getProperty(const char *propertyname) const override { return getWatchedDescriptor()->getProperty(propertyname); }

    virtual int getFieldCount() const override { return getWatchedObject() == nullptr ? 0 : getWatchedDescriptor()->getFieldCount(); }
    virtual const char *getFieldName(int field) const override { return getWatchedDescriptor()->getFieldName(field); }
    virtual unsigned int getFieldTypeFlags(int field) const override { return getWatchedDescriptor()->getFieldTypeFlags(field); }
    virtual const char *getFieldTypeString(int field) const override { return getWatchedDescriptor()->getFieldTypeString(field); }
    virtual const char *getFieldStructName(int field) const override { return getWatchedDescriptor()->getFieldStructName(field); }

    virtual const char **getFieldPropertyNames(int field) const override { return getWatchedDescriptor()->getFieldPropertyNames(field); }
    virtual const char *getFieldProperty(int field, const char *propertyname) const override { return getWatchedDescriptor()->getFieldProperty(field, propertyname); }

    // Each cObject[Ptr]Watch instance has its own descriptor instance, which must only describe that watch
    // instance only, because the type to act as (masquerade) depends on the concrete type pointed to by the
    // watched cObject pointer (captured by reference) at any given moment. And we have no access to any
    // (external) object pointers in the class-level query methods above.
    virtual std::string getFieldValueAsString(any_ptr object, int field, int i) const override {
        ASSERT(fromAnyPtr<cObject>(object) == watchToDescribe);
        cObject *watched = getWatchedObject();
        return watched ? watched->getDescriptor()->getFieldValueAsString(any_ptr(watched), field, i) : "n/a";
    }

    virtual void setFieldValueAsString(any_ptr object, int field, int i, const char *value) const override {
        ASSERT(fromAnyPtr<cObject>(object) == watchToDescribe);
        cObject *watched = getWatchedObject();
        if (watched)
            getWatchedDescriptor()->setFieldValueAsString(any_ptr(watched), field, i, value);
    }

    virtual cValue getFieldValue(any_ptr object, int field, int i) const override {
        ASSERT(fromAnyPtr<cObject>(object) == watchToDescribe);
        cObject *watched = getWatchedObject();
        return watched ? watched->getDescriptor()->getFieldValue(any_ptr(watched), field, i) : cValue();
    }

    virtual void setFieldValue(any_ptr object, int field, int i, const cValue& value) const override {
        ASSERT(fromAnyPtr<cObject>(object) == watchToDescribe);
        cObject *watched = getWatchedObject();
        if (watched)
            getWatchedDescriptor()->setFieldValue(any_ptr(watched), field, i, value);
    }

    virtual any_ptr getFieldStructValuePointer(any_ptr object, int field, int i) const override {
        ASSERT(fromAnyPtr<cObject>(object) == watchToDescribe);
        cObject *watched = getWatchedObject();
        return watched != nullptr ? getWatchedDescriptor()->getFieldStructValuePointer(any_ptr(watched), field, i) : any_ptr(nullptr);
    }

    virtual void setFieldStructValuePointer(any_ptr object, int field, int i, any_ptr ptr) const override {
        ASSERT(fromAnyPtr<cObject>(object) == watchToDescribe);
        cObject *watched = getWatchedObject();
        if (watched)
            getWatchedDescriptor()->setFieldStructValuePointer(any_ptr(watched), field, i, ptr);
    }

    virtual int getFieldArraySize(any_ptr object, int field) const override {
        ASSERT(fromAnyPtr<cObject>(object) == static_cast<cObject *>(watchToDescribe));
        cObject *watched = getWatchedObject();
        return watched ? getWatchedDescriptor()->getFieldArraySize(any_ptr(watched), field) : 0;
    }

    virtual void setFieldArraySize(any_ptr object, int field, int size) const override {
        ASSERT(fromAnyPtr<cObject>(object) == static_cast<cObject *>(watchToDescribe));
        cObject *watched = getWatchedObject();
        if (watched)
            getWatchedDescriptor()->setFieldArraySize(any_ptr(watched), field, size);
    }
};

// ----

/**
 * Internal. Wraps a cVisitor, in such a way that it prevents a single
 * cObject from being visited. Used to break reference loops when a
 * cWatch is pointed to its parent.
 */
class LoopCuttingVisitor : public cVisitor
{
  private:
    cVisitor *wrapped;
    cObject *skip;
  public:
    LoopCuttingVisitor(cVisitor *wrapped, cObject *skip)
      : wrapped(wrapped), skip(skip) { }

    virtual bool visit(cObject *obj) override {
        return obj == skip ? true : wrapped->visit(obj);
    }
};

// ----

cWatch_cObject::cWatch_cObject(const char *name, const char *typeName, cObject& ref)
    : cWatchBase(name), r(ref), desc(new cWatchProxyDescriptor(this)), typeName(typeName)
{
    take(desc);
}

void cWatch_cObject::forEachChild(cVisitor *visitor)
{
    LoopCuttingVisitor lcv(visitor, this);
    r.forEachChild(&lcv);
}

cWatch_cObjectPtr::cWatch_cObjectPtr(const char *name, const char *typeName, cObject *&ptr)
    : cWatchBase(name), rp(ptr), desc(new cWatchProxyDescriptor(this)), typeName(typeName)
{
    take(desc);
}

void cWatch_cObjectPtr::forEachChild(cVisitor *visitor)
{
    LoopCuttingVisitor lcv(visitor, this);
    if (rp)
        rp->forEachChild(&lcv);
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

