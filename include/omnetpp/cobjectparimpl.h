//==========================================================================
//   COBJECTPARIMPL.H  - part of
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

#ifndef __OMNETPP_COBJECTPARIMPL_H
#define __OMNETPP_COBJECTPARIMPL_H

#include "cparimpl.h"

namespace omnetpp {

class cTemporaryOwner;

namespace internal {

/**
 * @brief A cParImpl subclass that stores a module/channel parameter of type "object".
 *
 * Rules:
 * 1. The user cannot "take" the object from the cPar. It can only clone it if needs be.
 * 2. A volatile parameters's object value is replaced on each read operation.
 * 3. Plain cObject objects (non-cOwnedObject ones) are not permitted.
 * 4. cOwnedObject objects are "taken" ownership via take(), but only if their owner allows (i.e. is a soft owner).
 * 5. cOwnedObject objects are owned iff their owner is this object.
 * 6. nullptr is allowed as value.
 *
 * @ingroup Internals
 */
class SIM_API cObjectParImpl : public cParImpl
{
  protected:
    // selector: flags & FL_ISEXPR
    cExpression *expr = nullptr;
    mutable cObject *obj = nullptr; // stores the result of the last evaluation
    opp_staticpooledstring expectedType; // value of the "@class" property, or ""

  private:
    void copy(const cObjectParImpl& other);

  protected:
    void deleteOld();
    void deleteObject();
    virtual void doSetObject(cObject *object);
    virtual void checkType(cObject *object) const;
    virtual void checkOwnership(cObject *obj, cTemporaryOwner& tmp) const;

  public:
    /** @name Constructors, destructor, assignment. */
    //@{

    /**
     * Constructor.
     */
    explicit cObjectParImpl() {}

    /**
     * Copy constructor.
     */
    cObjectParImpl(const cObjectParImpl& other) : cParImpl(other) {copy(other);}

    /**
     * Destructor.
     */
    virtual ~cObjectParImpl();

    /**
     * Assignment operator.
     */
    void operator=(const cObjectParImpl& otherpar);
    //@}

    /** @name Redefined cObject member functions */
    //@{

    /**
     * Creates and returns an exact copy of this object.
     */
    virtual cObjectParImpl *dup() const override  {return new cObjectParImpl(*this);}
    //@}

    /** @name Redefined cParImpl setter functions. */
    //@{

    /**
     * Raises an error: cannot convert bool to object.
     */
    virtual void setBoolValue(bool b) override;

    /**
     * Raises an error: cannot convert integer to object.
     */
    virtual void setIntValue(intval_t l) override;

    /**
     * Raises an error: cannot convert double to object.
     */
    virtual void setDoubleValue(double d) override;

    /**
     * Raises an error: cannot convert string to object.
     */
    virtual void setStringValue(const char *s) override;

    /**
     * Sets the value to the given object.
     */
    virtual void setObjectValue(cObject *object) override;

    /**
     * Raises an error: cannot convert XML to object.
     */
    virtual void setXMLValue(cXMLElement *node) override;

    /**
     * Sets the value to the given expression. This object will
     * assume the responsibility to delete the expression object.
     */
    virtual void setExpression(cExpression *e) override;
    //@}

    /** @name Redefined cParImpl getter functions. */
    //@{

    /**
     * Raises an error: cannot convert object to bool.
     */
    virtual bool boolValue(cComponent *context) const override;

    /**
     * Raises an error: cannot convert object to integer.
     */
    virtual intval_t intValue(cComponent *context) const override;

    /**
     * Raises an error: cannot convert object to double.
     */
    virtual double doubleValue(cComponent *context) const override;

    /**
     * Raises an error: cannot convert object to string.
     */
    virtual const char *stringValue(cComponent *context) const override;

    /**
     * Raises an error: cannot convert object to string.
     */
    virtual std::string stdstringValue(cComponent *context) const override;

    /**
     * Returns the value of the parameter.
     */
    virtual cObject *objectValue(cComponent *context) const override;

    /**
     * Raises an error: cannot convert object to XML.
     */
    virtual cXMLElement *xmlValue(cComponent *context) const override;

    /**
     * Returns pointer to the expression stored by the object, or nullptr.
     */
    virtual cExpression *getExpression() const override;
    //@}

    /** @name Type, prompt text, input flag, change flag. */
    //@{

    /**
     * Returns object.
     */
    virtual Type getType() const override;

    /**
     * Returns false.
     */
    virtual bool isNumeric() const override;

    /**
     * Sets the expected type ("@class" property in NED)
     */
    virtual void setExpectedType(const char *s);
    //@}

    /** @name Redefined cParImpl misc functions. */
    //@{
    /**
     * Replaces for non-const values, replaces the stored expression with its
     * evaluation.
     */
    virtual void convertToConst(cComponent *context) override;

    /**
     * Returns the value in text form.
     */
    virtual std::string str() const override;

    /**
     * Converts from text.
     */
    virtual void parse(const char *text, FileLine loc) override;

    /**
     * Object comparison.
     */
    virtual int compare(const cParImpl *other) const override;

    /**
     * Visits the object element
     */
    virtual void forEachChild(cVisitor *v, cComponent *context) override;
    //@}
};

}  // namespace internal
}  // namespace omnetpp

#endif


