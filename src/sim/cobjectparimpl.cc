//==========================================================================
//   COBJECTPARIMPL.CC  - part of
//                     OMNeT++/OMNEST
//            Discrete System Simulation in C++
//
//==========================================================================

/*--------------------------------------------------------------*
  Copyright (C) 1992-2017 Andras Varga
  Copyright (C) 2006-2017 OpenSim Ltd.

  This file is distributed WITHOUT ANY WARRANTY. See the file
  `license' for details on this and other legal matters.
*--------------------------------------------------------------*/

#include "omnetpp/cobjectparimpl.h"
#include "omnetpp/cstringtokenizer.h"
#include "omnetpp/cdynamicexpression.h"
#include "omnetpp/cobjectfactory.h"
#include "omnetpp/ccomponent.h"
#include "omnetpp/cvaluearray.h"
#include "omnetpp/cvaluemap.h"
#include "omnetpp/cvalueholder.h"
#include "ctemporaryowner.h"
#include "common/stringutil.h"

using namespace omnetpp::common;

namespace omnetpp {
namespace internal {

cObjectParImpl::~cObjectParImpl()
{
    deleteOld();
}

void cObjectParImpl::copy(const cObjectParImpl& other)
{
    if (flags & FL_ISEXPR)
        expr = other.expr->dup();

    expectedType = other.expectedType;

    obj = other.obj;
    if (obj) {
        if (!obj->isOwnedObject())
            obj = obj->dup();
        else if (obj->getOwner() == &other)
            take(static_cast<cOwnedObject*>(obj = obj->dup()));
    }
}

void cObjectParImpl::operator=(const cObjectParImpl& other)
{
    if (this == &other)
        return;
    deleteOld();
    cParImpl::operator=(other);
    copy(other);
}

void cObjectParImpl::forEachChild(cVisitor *v, cComponent *context)
{
    if (isSet() && !isExpression()) {
        if (obj)
            v->visit(obj);
    }
}

void cObjectParImpl::setBoolValue(bool b)
{
    throw cRuntimeError(this, E_BADASSIGN, "bool", "object");
}

void cObjectParImpl::setIntValue(intval_t l)
{
    throw cRuntimeError(this, E_BADASSIGN, "integer", "object");
}

void cObjectParImpl::setDoubleValue(double d)
{
    throw cRuntimeError(this, E_BADASSIGN, "double", "object");
}

void cObjectParImpl::setStringValue(const char *s)
{
    throw cRuntimeError(this, E_BADASSIGN, "string", "object");
}

void cObjectParImpl::setObjectValue(cObject *object)
{
    deleteOld();
    doSetObject(object);
    flags |= FL_CONTAINSVALUE | FL_ISSET;
    checkType(object);
}

void cObjectParImpl::setXMLValue(cXMLElement *node)
{
    throw cRuntimeError(this, E_BADASSIGN, "XML", "object");
}

void cObjectParImpl::setExpression(cExpression *e)
{
    deleteOld();
    expr = e;
    flags |= FL_ISEXPR | FL_CONTAINSVALUE | FL_ISSET;
}

bool cObjectParImpl::boolValue(cComponent *) const
{
    throw cRuntimeError(this, E_BADCAST, "object", "bool");
}

intval_t cObjectParImpl::intValue(cComponent *) const
{
    throw cRuntimeError(this, E_BADCAST, "object", "integer");
}

double cObjectParImpl::doubleValue(cComponent *) const
{
    throw cRuntimeError(this, E_BADCAST, "object", "double");
}

const char *cObjectParImpl::stringValue(cComponent *) const
{
    throw cRuntimeError(this, E_BADCAST, "object", "string");
}

std::string cObjectParImpl::stdstringValue(cComponent *) const
{
    throw cRuntimeError(this, E_BADCAST, "object", "string");
}

static void throwNotOwned(cObject *obj, cObject *parent=nullptr)
{
    ASSERT(parent == nullptr || obj->getOwner() != parent);

    std::string msg = opp_stringf("Object parameters may only hold objects they fully own, but object %s", obj->getClassAndFullName().c_str());

    std::string note;
    if (parent != nullptr)
        note = opp_stringf(" inside the result object (parent: %s)", parent->getClassAndFullName().c_str());

    std::string reason;
    if (!obj->isOwnedObject())
        reason = " is not a cOwnedObject and does not keep track of its owner";
    else if (obj->getOwner() == nullptr)
        reason = " has been removed from the ownership hierarchy (owner=nullptr)";
    else if (dynamic_cast<cParImpl*>(obj->getOwner()) != nullptr)
        reason = opp_stringf(" is already owned by a parameter named '%s'", obj->getOwner()->getName());
    else
        reason = opp_stringf(" is already owned by %s", obj->getOwner()->getClassAndFullPath().c_str());

    std::string hint = " (Hint: Try cloning it using the dup() function!)";

    throw cRuntimeError("%s", (msg + note + reason + hint).c_str());
}

namespace {

class OwnershipCheckerVisitor : public cVisitor
{
  private:
    cObject *parent;
  public:
    OwnershipCheckerVisitor(cObject *parent) : parent(parent) {}

    void checkValue(cObject *obj, const cValue& value) {
        if (value.getType() == cValue::POINTER && !value.pointerValue().contains<cObject>())
            throw cRuntimeError("Object parameters may only hold objects they fully own, but a value in %s contains a non-cObject pointer (%s)",
                    obj->getClassAndFullName().c_str(), value.pointerValue().str().c_str());
    }

    void checkObject(cObject *obj) {
        // allow fully owned objects (safe) and modules/channels (relatively safe)
        if (obj->getOwner() != parent)
            throwNotOwned(obj, parent);

        // disallow non-cObject pointers in cValues
        if (cValueArray *array = dynamic_cast<cValueArray*>(obj)) {
            for (const auto& value : array->getArray())
                checkValue(array, value);
        }
        else if (cValueMap *map = dynamic_cast<cValueMap*>(obj)) {
            for (const auto& entry: map->getFields())
                checkValue(map, entry.second);
        }
        else if (cValueHolder *holder = dynamic_cast<cValueHolder*>(obj)) {
            checkValue(holder, holder->get());
        }
    }

    virtual bool visit(cObject *obj) override {
        checkObject(obj);
        cObject *oldParent = parent;
        parent = obj;
        obj->forEachChild(this);
        parent = oldParent;
        return true;
    }
};

} // namespace

void cObjectParImpl::checkOwnership(cObject *obj, cTemporaryOwner& tmp) const
{
    // This is the place where we ensure that only cOwnedObjects that we can own
    // are permitted. Plain cObjects are not allowed, because the user could make
    // it crash even from NED, by letting one NED object parameter reference another
    // object parameter whose value can change. See e.g. NED_jsonparam_ownership_1.test
    if (obj->getOwner() != &tmp)
        throwNotOwned(obj);

    // Check for external references contained within the object. They cannot
    // be detected with 100% accuracy (this is C++ after all), our best bet is to
    // traverse its contents via forEach(), and check that objects in the tree
    // are fully owned by their parents.
    OwnershipCheckerVisitor visitor(obj);
    obj->forEachChild(&visitor);
}

cObject *cObjectParImpl::objectValue(cComponent *context) const
{
    if ((flags & FL_ISSET) == 0)
        throw cRuntimeError(E_PARNOTSET);

    if ((flags & FL_ISEXPR) == 0)
        return obj;
    else {
        try {
            cTemporaryOwner tmp(cTemporaryOwner::DestructorMode::DISPOSE);
            cValue value = evaluate(expr, context);
            cObject *obj = (value.containsObject() || value.isNullptr()) ? value.objectValue() : new cValueHolder("holder", value);
            if (obj)
                checkOwnership(obj, tmp);
            cObjectParImpl *mutableThis = const_cast<cObjectParImpl*>(this);
            mutableThis->deleteObject();
            mutableThis->doSetObject(obj);
            checkType(obj);
            return obj;
        }
        catch (std::exception& e) {
            throw cRuntimeError(e, expr->getSourceLocation().c_str());
        }
    }
}

cXMLElement *cObjectParImpl::xmlValue(cComponent *) const
{
    throw cRuntimeError(this, E_BADCAST, "object", "XML");
}

cExpression *cObjectParImpl::getExpression() const
{
    return (flags & FL_ISEXPR) ? expr : nullptr;
}

void cObjectParImpl::deleteOld()
{
    cParImpl::deleteOld(expr);
    deleteObject();
}

void cObjectParImpl::deleteObject()
{
    if (obj) {
        if (!obj->isOwnedObject())
            delete obj;
        else if (obj->getOwner() == this)
            dropAndDelete(static_cast<cOwnedObject *>(obj));
        obj = nullptr;
    }
}

void cObjectParImpl::doSetObject(cObject *object)
{
    obj = object;
    if (cOwnedObject *ownedObj = dynamic_cast<cOwnedObject*>(obj))
        if (ownedObj->getOwner()->isSoftOwner())
            take(ownedObj);
}

void cObjectParImpl::checkType(cObject *object) const
{
    if (expectedType.empty())
        return;

    const char *typeSpec = expectedType.c_str();
    bool optional = typeSpec[strlen(typeSpec)-1] == '?';
    if (!object) {
        if (optional)
            return;
        else
            throw cRuntimeError("nullptr is not an allowed value (append '?' to the name in @class() to allow it)");
    }

    std::string typeName = optional ? std::string(typeSpec, strlen(typeSpec)-1) : typeSpec; // remove trailing '?'
    cObjectFactory *factory = cObjectFactory::find(typeName.c_str());
    if (!factory)
        throw cRuntimeError("'%s' is not a registered class (missing Register_Class() or Register_Abstract_Class() macro?)", typeName.c_str());
    if (!factory->isInstance(object))
        throw cRuntimeError("Cannot cast object (%s)%s to '%s', the C++ type declared for the parameter",
                object->getClassName(), object->getFullName(), factory->getFullName());

}

cPar::Type cObjectParImpl::getType() const
{
    return cPar::OBJECT;
}

bool cObjectParImpl::isNumeric() const
{
    return false;
}

void cObjectParImpl::setExpectedType(const char *s)
{
    expectedType = s;
}

void cObjectParImpl::convertToConst(cComponent *context)
{
    cObject *saved = objectValue(context);
    auto loc = getSourceLocation();
    obj = nullptr;
    setObjectValue(saved);
    setSourceLocation(loc);
}

std::string cObjectParImpl::str() const
{
    if (flags & FL_ISEXPR)
        return expr->str();
    else if (!obj)
        return "nullptr";
    else if (dynamic_cast<cValueArray*>(obj) || dynamic_cast<cValueMap*>(obj))
        return obj->str();
    else if (dynamic_cast<cOwnedDynamicExpression*>(obj))
        return "expr(" + obj->str() + ")";
    else
        return obj->getClassAndFullName() + ": " + obj->str(); // non-parseable...
}

void cObjectParImpl::parse(const char *text, FileLine loc)
{
    // try parsing it as an expression
    cDynamicExpression *dynexpr = new cDynamicExpression();
    try {
        dynexpr->parseNedExpr(text);
    }
    catch (std::exception& e) {
        delete dynexpr;
        throw;
    }
    dynexpr->setSourceLocation(loc);
    setExpression(dynexpr);

    // simplify if possible: store as constant instead of expression
    if (dynexpr->isAConstant())
        convertToConst(nullptr);

    setSourceLocation(loc);
}

int cObjectParImpl::compare(const cParImpl *other) const
{
    int ret = cParImpl::compare(other);
    if (ret != 0)
        return ret;

    const cObjectParImpl *other2 = dynamic_cast<const cObjectParImpl *>(other);
    if (flags & FL_ISEXPR)
        return expr->compare(other2->expr);
    else
        return (obj == other2->obj) ? 0 : (obj < other2->obj) ? -1 : 1;
}

}  // namespace internal
}  // namespace omnetpp

