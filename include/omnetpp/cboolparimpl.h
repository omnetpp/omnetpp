//==========================================================================
//   CBOOLPARIMPL.H  - part of
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

#ifndef __OMNETPP_CBOOLPARIMPL_H
#define __OMNETPP_CBOOLPARIMPL_H

#include "cparimpl.h"

namespace omnetpp {
namespace internal {

/**
 * @brief A cParImpl subclass that stores a module/channel parameter
 * of the type bool.
 *
 * @ingroup Internals
 */
class SIM_API cBoolParImpl : public cParImpl
{
  protected:
    // selector: flags & FL_ISEXPR
    union {
      cExpression *expr;
      bool val;
    };

  private:
    void copy(const cBoolParImpl& other);

  protected:
    void deleteOld();

  public:
    /** @name Constructors, destructor, assignment. */
    //@{

    /**
     * Constructor.
     */
    explicit cBoolParImpl();

    /**
     * Copy constructor.
     */
    cBoolParImpl(const cBoolParImpl& other) : cParImpl(other) {copy(other);}

    /**
     * Destructor.
     */
    virtual ~cBoolParImpl();

    /**
     * Assignment operator.
     */
    void operator=(const cBoolParImpl& otherpar);
    //@}

    /** @name Redefined cObject member functions */
    //@{

    /**
     * Creates and returns an exact copy of this object.
     */
    virtual cBoolParImpl *dup() const override  {return new cBoolParImpl(*this);}
    //@}

    /** @name Redefined cParImpl setter functions. */
    //@{

    /**
     * Sets the value to the given constant.
     */
    virtual void setBoolValue(bool b) override;

    /**
     * Raises an error: cannot convert integer to bool.
     */
    virtual void setIntValue(intval_t l) override;

    /**
     * Raises an error: cannot convert double to bool.
     */
    virtual void setDoubleValue(double d) override;

    /**
     * Raises an error: cannot convert string to bool.
     */
    virtual void setStringValue(const char *s) override;

    /**
     * Raises an error: cannot convert object to bool.
     */
    virtual void setObjectValue(cObject *object) override;

    /**
     * Raises an error: cannot convert XML to bool.
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
     * Returns the value of the parameter.
     */
    virtual bool boolValue(cComponent *context) const override;

    /**
     * Raises an error: cannot convert bool to integer.
     */
    virtual intval_t intValue(cComponent *context) const override;

    /**
     * Raises an error: cannot convert bool to double.
     */
    virtual double doubleValue(cComponent *context) const override;

    /**
     * Raises an error: cannot convert bool to string.
     */
    virtual const char *stringValue(cComponent *context) const override;

    /**
     * Raises an error: cannot convert bool to string.
     */
    virtual std::string stdstringValue(cComponent *context) const override;

    /**
     * Raises an error: cannot convert bool to object.
     */
    virtual cObject *objectValue(cComponent *context) const override;

    /**
     * Raises an error: cannot convert bool to XML.
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
     * Returns BOOL.
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


