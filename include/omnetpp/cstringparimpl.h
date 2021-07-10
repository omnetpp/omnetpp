//==========================================================================
//   CSTRINGPARIMPL.H  - part of
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

#ifndef __OMNETPP_CSTRINGPARIMPL_H
#define __OMNETPP_CSTRINGPARIMPL_H

#include "cparimpl.h"

namespace omnetpp {
namespace internal {

/**
 * @brief A cParImpl subclass that stores a module/channel parameter
 * of the type string.
 *
 * @ingroup Internals
 */
class SIM_API cStringParImpl : public cParImpl
{
  protected:
    // only one of expr and val is active at a time; however, we cannot use
    // union here because std::string has constructor/destructor.
    // selector: flags & FL_ISEXPR
    cExpression *expr = nullptr;
    std::string val;

  private:
    void copy(const cStringParImpl& other);

  protected:
    void deleteOld();

  public:
    /** @name Constructors, destructor, assignment. */
    //@{

    /**
     * Constructor.
     */
    explicit cStringParImpl() {}

    /**
     * Copy constructor.
     */
    cStringParImpl(const cStringParImpl& other) : cParImpl(other) {copy(other);}

    /**
     * Destructor.
     */
    virtual ~cStringParImpl();

    /**
     * Assignment operator.
     */
    void operator=(const cStringParImpl& otherpar);
    //@}

    /** @name Redefined cObject member functions */
    //@{

    /**
     * Creates and returns an exact copy of this object.
     */
    virtual cStringParImpl *dup() const override  {return new cStringParImpl(*this);}
    //@}

    /** @name Redefined cParImpl setter functions. */
    //@{

    /**
     * Raises an error: cannot convert bool to string.
     */
    virtual void setBoolValue(bool b) override;

    /**
     * Raises an error: cannot convert integer to string.
     */
    virtual void setIntValue(intval_t l) override;

    /**
     * Raises an error: cannot convert double to string.
     */
    virtual void setDoubleValue(double d) override;

    /**
     * Sets the value to the given string. nullptr is accepted as "".
     */
    virtual void setStringValue(const char *s) override;

    /**
     * Raises an error: cannot convert object to string.
     */
    virtual void setObjectValue(cObject *object) override;

    /**
     * Raises an error: cannot convert XML to string.
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
     * Raises an error: cannot convert string to bool.
     */
    virtual bool boolValue(cComponent *context) const override;

    /**
     * Raises an error: cannot convert string to integer.
     */
    virtual intval_t intValue(cComponent *context) const override;

    /**
     * Raises an error: cannot convert string to double.
     */
    virtual double doubleValue(cComponent *context) const override;

    /**
     * Returns the value of the parameter.
     */
    virtual const char *stringValue(cComponent *context) const override;

    /**
     * Returns the value of the parameter.
     */
    virtual std::string stdstringValue(cComponent *context) const override;

    /**
     * Raises an error: cannot convert string to object.
     */
    virtual cObject *objectValue(cComponent *context) const override;

    /**
     * Raises an error: cannot convert string to XML.
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
     * Returns STRING.
     */
    virtual Type getType() const override;

    /**
     * Returns false.
     */
    virtual bool isNumeric() const override;
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
    //@}
};

}  // namespace internal
}  // namespace omnetpp

#endif


