//==========================================================================
//   CLONGPARIMPL.H  - part of
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

#ifndef __OMNETPP_CLONGPARIMPL_H
#define __OMNETPP_CLONGPARIMPL_H

#include "cparimpl.h"

namespace omnetpp {

/**
 * @brief A cParImpl subclass that stores a module/channel parameter
 * of the type long.
 *
 * @ingroup Internals
 */
class SIM_API cLongParImpl : public cParImpl
{
  protected:
    // selector: flags & FL_ISEXPR
    union {
      cExpression *expr;
      long val;
    };

  private:
    void copy(const cLongParImpl& other);

  protected:
    void deleteOld();

  public:
    /** @name Constructors, destructor, assignment. */
    //@{

    /**
     * Constructor.
     */
    explicit cLongParImpl();

    /**
     * Copy constructor.
     */
    cLongParImpl(const cLongParImpl& other) : cParImpl(other) {copy(other);}

    /**
     * Destructor.
     */
    virtual ~cLongParImpl();

    /**
     * Assignment operator.
     */
    void operator=(const cLongParImpl& otherpar);
    //@}

    /** @name Redefined cObject member functions */
    //@{

    /**
     * Creates and returns an exact copy of this object.
     */
    virtual cLongParImpl *dup() const override  {return new cLongParImpl(*this);}

    /**
     * Serializes the object into a buffer.
     */
    virtual void parsimPack(cCommBuffer *buffer) const override;

    /**
     * Deserializes the object from a buffer.
     */
    virtual void parsimUnpack(cCommBuffer *buffer) override;
    //@}

    /** @name Redefined cParImpl setter functions. */
    //@{

    /**
     * Raises an error: cannot convert bool to long.
     */
    virtual void setBoolValue(bool b) override;

    /**
     * Sets the value to the given constant.
     */
    virtual void setLongValue(long l) override;

    /**
     * Converts from double.
     */
    virtual void setDoubleValue(double d) override;

    /**
     * Raises an error: cannot convert string to long.
     */
    virtual void setStringValue(const char *s) override;

    /**
     * Raises an error: cannot convert XML to long.
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
     * Raises an error: cannot convert long to bool.
     */
    virtual bool boolValue(cComponent *context) const override;

    /**
     * Returns the value of the parameter.
     */
    virtual long longValue(cComponent *context) const override;

    /**
     * Converts the value to double.
     */
    virtual double doubleValue(cComponent *context) const override;

    /**
     * Raises an error: cannot convert long to string.
     */
    virtual const char *stringValue(cComponent *context) const override;

    /**
     * Raises an error: cannot convert long to string.
     */
    virtual std::string stdstringValue(cComponent *context) const override;

    /**
     * Raises an error: cannot convert long to XML.
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
     * Returns LONG.
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
    virtual void parse(const char *text) override;

    /**
     * Object comparison.
     */
    virtual int compare(const cParImpl *other) const override;
    //@}
};

}  // namespace omnetpp


#endif


