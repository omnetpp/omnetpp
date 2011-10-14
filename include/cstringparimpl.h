//==========================================================================
//   CSTRINGPARIMPL.H  - part of
//                     OMNeT++/OMNEST
//            Discrete System Simulation in C++
//
//  Author: Andras Varga
//
//==========================================================================

/*--------------------------------------------------------------*
  Copyright (C) 1992-2008 Andras Varga
  Copyright (C) 2006-2008 OpenSim Ltd.

  This file is distributed WITHOUT ANY WARRANTY. See the file
  `license' for details on this and other legal matters.
*--------------------------------------------------------------*/

#ifndef __CSTRINGPARIMPL_H
#define __CSTRINGPARIMPL_H

#include "cparimpl.h"

NAMESPACE_BEGIN

/**
 * A cParImpl subclass that stores a module/channel parameter
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
    cExpression *expr;
    std::string val;

  private:
    void copy(const cStringParImpl& other);

  protected:
    std::string evaluate(cComponent *context) const;
    void deleteOld();

  public:
    /** @name Constructors, destructor, assignment. */
    //@{

    /**
     * Constructor.
     */
    explicit cStringParImpl();

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
    virtual cStringParImpl *dup() const  {return new cStringParImpl(*this);}

    /**
     * Serializes the object into a buffer.
     */
    virtual void parsimPack(cCommBuffer *buffer);

    /**
     * Deserializes the object from a buffer.
     */
    virtual void parsimUnpack(cCommBuffer *buffer);
    //@}

    /** @name Redefined cParImpl setter functions. */
    //@{

    /**
     * Raises an error: cannot convert bool to string.
     */
    virtual void setBoolValue(bool b);

    /**
     * Raises an error: cannot convert long to string.
     */
    virtual void setLongValue(long l);

    /**
     * Raises an error: cannot convert double to string.
     */
    virtual void setDoubleValue(double d);

    /**
     * Sets the value to the given string. NULL is accepted as "".
     */
    virtual void setStringValue(const char *s);

    /**
     * Raises an error: cannot convert XML to long.
     */
    virtual void setXMLValue(cXMLElement *node);

    /**
     * Sets the value to the given expression. This object will
     * assume the responsibility to delete the expression object.
     */
    virtual void setExpression(cExpression *e);
    //@}

    /** @name Redefined cParImpl getter functions. */
    //@{

    /**
     * Raises an error: cannot convert string to bool.
     */
    virtual bool boolValue(cComponent *context) const;

    /**
     * Raises an error: cannot convert string to long.
     */
    virtual long longValue(cComponent *context) const;

    /**
     * Raises an error: cannot convert string to double.
     */
    virtual double doubleValue(cComponent *context) const;

    /**
     * Returns the value of the parameter.
     */
    virtual const char *stringValue(cComponent *context) const;

    /**
     * Returns the value of the parameter.
     */
    virtual std::string stdstringValue(cComponent *context) const;

    /**
     * Raises an error: cannot convert string to XML.
     */
    virtual cXMLElement *xmlValue(cComponent *context) const;

    /**
     * Returns pointer to the expression stored by the object, or NULL.
     */
    virtual cExpression *getExpression() const;
    //@}

    /** @name Type, prompt text, input flag, change flag. */
    //@{

    /**
     * Returns STRING.
     */
    virtual Type getType() const;

    /**
     * Returns false.
     */
    virtual bool isNumeric() const;
    //@}

    /** @name Redefined cParImpl misc functions. */
    //@{

    /**
     * Replaces for non-const values, replaces the stored expression with its
     * evaluation.
     */
    virtual void convertToConst(cComponent *context);

    /**
     * Returns the value in text form.
     */
    virtual std::string str() const;

    /**
     * Converts from text.
     */
    virtual void parse(const char *text);

    /**
     * Object comparison.
     */
    virtual int compare(const cParImpl *other) const;
    //@}
};

NAMESPACE_END


#endif


