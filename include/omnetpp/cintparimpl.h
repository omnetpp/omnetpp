//==========================================================================
//   CINTPARIMPL.H  - part of
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

#ifndef __OMNETPP_CINTPARIMPL_H
#define __OMNETPP_CINTPARIMPL_H

#include "cparimpl.h"

namespace omnetpp {
namespace internal {

/**
 * @brief A cParImpl subclass that stores an integer module/channel parameter.
 *
 * @ingroup Internals
 */
class SIM_API cIntParImpl : public cParImpl
{
  protected:
    // selector: flags & FL_ISEXPR
    union {
      cExpression *expr;
      intval_t val;
    };

  private:
    void copy(const cIntParImpl& other);

  protected:
    void deleteOld() {cParImpl::deleteOld(expr);}

  public:
    /** @name Constructors, destructor, assignment. */
    //@{

    /**
     * Constructor.
     */
    explicit cIntParImpl();

    /**
     * Copy constructor.
     */
    cIntParImpl(const cIntParImpl& other) : cParImpl(other) {copy(other);}

    /**
     * Destructor.
     */
    virtual ~cIntParImpl();

    /**
     * Assignment operator.
     */
    void operator=(const cIntParImpl& otherpar);
    //@}

    /** @name Redefined cObject member functions */
    //@{

    /**
     * Creates and returns an exact copy of this object.
     */
    virtual cIntParImpl *dup() const override  {return new cIntParImpl(*this);}
    //@}

    /** @name Redefined cParImpl setter functions. */
    //@{

    /**
     * Raises an error: cannot convert bool to integer.
     */
    virtual void setBoolValue(bool b) override;

    /**
     * Sets the value to the given integer.
     */
    virtual void setIntValue(intval_t l) override;

    /**
     * Converts from double.
     */
    virtual void setDoubleValue(double d) override;

    /**
     * Raises an error: cannot convert string to integer.
     */
    virtual void setStringValue(const char *s) override;

    /**
     * Raises an error: cannot convert object to integer.
     */
    virtual void setObjectValue(cObject *object) override;

    /**
     * Raises an error: cannot convert XML to integer.
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
     * Raises an error: cannot convert integer to bool.
     */
    virtual bool boolValue(cComponent *context) const override;

    /**
     * Returns the value of the parameter.
     */
    virtual intval_t intValue(cComponent *context) const override;

    /**
     * Converts the value to double.
     */
    virtual double doubleValue(cComponent *context) const override;

    /**
     * Raises an error: cannot convert integer to string.
     */
    virtual const char *stringValue(cComponent *context) const override;

    /**
     * Raises an error: cannot convert integer to string.
     */
    virtual std::string stdstringValue(cComponent *context) const override;

    /**
     * Raises an error: cannot convert integer to object.
     */
    virtual cObject *objectValue(cComponent *context) const override;

    /**
     * Raises an error: cannot convert integer to XML.
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
     * Returns INT.
     */
    virtual Type getType() const override;

    /**
     * Returns true.
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


