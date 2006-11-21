//==========================================================================
//   CPARVALUE.H  - part of
//                     OMNeT++/OMNEST
//            Discrete System Simulation in C++
//
//  Author: Andras Varga
//
//==========================================================================

/*--------------------------------------------------------------*
  Copyright (C) 1992-2005 Andras Varga

  This file is distributed WITHOUT ANY WARRANTY. See the file
  `license' for details on this and other legal matters.
*--------------------------------------------------------------*/

#ifndef __CPARVALUE_H
#define __CPARVALUE_H

#include "cpar.h"
#include "cexpression.h"
#include "cexception.h"

class cExpression;
class cXMLElement;
class cProperties;
class cComponent;


/**
 * FIXME revise docu in the whole class!!!!!!
 *
 * cParValue is an abstract base class for storing the values of module
 * (or channel) parameters. cParValue supports several data types via subclasses:
 * cLongPar, cDoublePar, cBoolPar, cStringPar, cXMLPar.
 *
 * @ingroup SimCore
 */
class SIM_API cParValue : public cNamedObject
{
  protected:
    // various flags, stored in cNamedObject::flags
    enum {
      FL_ISVOLATILE = 4,  // whether it was declared as "volatile" in NED
      FL_HASVALUE = 16,   // whether it has a value
      FL_ISEXPR = 32,     // whether it stores a constant or an expression
      FL_HASCHANGED = 64  // whether it has changed since last asked
    };

  public:
    typedef cPar::Type Type;

  public:
    // internal: sets the ISVOLATILE flag; NOTE: may be necessary to invoke convertToConst(cComponent *context) as well!
    virtual void setIsVolatile(bool f) {if (f) flags|=FL_ISVOLATILE; else flags&=~FL_ISVOLATILE;}

    // internal: create a parameter object representing the given type
    static cParValue *createWithType(Type type);

  public:
    /** @name Constructors, destructor, assignment. */
    //@{

    /**
     * Constructor.
     */
    explicit cParValue();

    /**
     * Copy constructor.
     */
    cParValue(const cParValue& other);

    /**
     * Destructor.
     */
    virtual ~cParValue();

    /**
     * Assignment operator.
     */
    cParValue& operator=(const cParValue& otherpar);
    //@}

    /** @name Redefined cOwnedObject functions */
    //@{
    /**
     * Redefined change return type to cParValue.
     */
    virtual cParValue *dup() const;

    /**
     * Serializes the object into a buffer.
     */
    virtual void netPack(cCommBuffer *buffer);

    /**
     * Deserializes the object from a buffer.
     */
    virtual void netUnpack(cCommBuffer *buffer);
    //@}

    /** @name Owner component, type, flags. */
    //@{
    /**
     * Returns the parameter type.
     */
    virtual Type type() const = 0;

    /**
     * Returns true if the stored value is of a numeric type.
     */
    virtual bool isNumeric() const = 0;

    /**
     * Returns true if this parameter is marked in the NED file as "function".
     * This flag affects the operation of setExpression().
     */
    virtual bool isVolatile() const {return flags & FL_ISVOLATILE;}

    /**
     * Returns true if the stored value is a constant, anf false if it is
     * an expression. (It is not examined whether the expression yields
     * a constant value or not.)
     */
    virtual bool isConstant() const {return !(flags & FL_ISEXPR);}

    /**
     * Returns true if the parameter has a default value.
     */
    virtual bool hasValue() const  {return flags & FL_HASVALUE;}

    /**
     * Returns true if the value has changed since the last changed() call.
     * Side effect: clears the 'changed' flag, so a next call will return
     * false.
     */
    virtual bool changed();
    //@}

    /** @name Setter functions. Note that overloaded assignment operators also exist. */
    //@{

    /**
     * Sets the value to the given bool value.
     */
    virtual void setBoolValue(bool b) = 0;

    /**
     * Sets the value to the given long value.
     */
    virtual void setLongValue(long l) = 0;

    /**
     * Sets the value to the given double value.
     */
    virtual void setDoubleValue(double d) = 0;

    /**
     * Sets the value to the given string value.
     * The cParValue will make its own copy of the string. NULL is also accepted
     * and treated as an empty string.
     */
    virtual void setStringValue(const char *s) = 0;

    /**
     * Sets the value to the given string value.
     */
    virtual void setStringValue(const std::string& s)  {setStringValue(s.c_str());}

    /**
     * Sets the value to the given cXMLElement.
     */
    virtual void setXMLValue(cXMLElement *node) = 0;

    /**
     * Sets the value to the given expression. This object will assume
     * the responsibility to delete the expression object.
     *
     * Note: if the parameter is marked as non-volatile (isVolatile()==false),
     * one should not set an expression as value. This is not enforced
     * by cParValue though.
     */
    virtual void setExpression(cExpression *e) = 0;
    //@}

    /** @name Getter functions. Note that overloaded conversion operators also exist. */
    //@{

    /**
     * Returns value as a boolean. The cParValue type must be bool (B) or a numeric type.
     */
    virtual bool boolValue(cComponent *context) const = 0;

    /**
     * Returns value as long. The cParValue type must be types long (L),
     * double (D), Boolean (B), function (F), distribution (T),
     * compiled expression (C) or expression (X).
     */
    virtual long longValue(cComponent *context) const = 0;

    /**
     * Returns value as double. The cParValue type must be types long (L),
     * double (D), function (F), Boolean (B), distribution (T),
     * compiled expression (C) or expression (X).
     */
    virtual double doubleValue(cComponent *context) const = 0;

    /**
     * Returns value as const char *. Only for string (S) type.
     * This method may can only be invoked when the parameter's value is a
     * string constant and not the result of expression evaluation (otherwise
     * an error is thrown). This practically means this method cannot be used
     * on parameters declared as "volatile string" in NED; they can only be
     * accessed using stdstringValue().
     */
    virtual const char *stringValue(cComponent *context) const = 0;

    /**
     * Returns value as string. Only for string (S) type.
     */
    virtual std::string stdstringValue(cComponent *context) const = 0;

    /**
     * Returns value as pointer to cXMLElement. The cParValue type must be XML (M).
     */
    virtual cXMLElement *xmlValue(cComponent *context) const = 0;

    /**
     * Returns pointer to the expression stored by the object, or NULL.
     */
    virtual cExpression *expression() const = 0;
    //@}

    /** @name Miscellaneous utility functions. */
    //@{

    /**
     * Replaces for non-const values, replaces the stored expression with its
     * evaluation.
     */
    virtual void convertToConst(cComponent *context) = 0;

    /**
     * Returns the value in text form.
     */
    virtual std::string toString() const = 0;

    /**
     * Convert the value from string, and store the result.
     * Returns true on success, false otherwise. No error message is generated.
     */
    virtual bool parse(const char *text) = 0;
    //@}

    /** @name Compare functions */
    //@{

    /**
     * Evaluates both expressions and compares the resulting values.
     * If they are of different types (type()), false is returned
     * without attempting to compare the values.
     */
    virtual bool equals(cParValue& other, cComponent *thiscontext, cComponent *othercontext);
    //@}
};

#endif


