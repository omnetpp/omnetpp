//==========================================================================
//   CPAR.H  - part of
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

#ifndef __CPAR_H
#define __CPAR_H

#include "cobject.h"
#include "cexpression.h"
#include "cexception.h"

class cExpression;
class cXMLElement;
class cProperties;
class cComponent;


/**
 * FIXME revise docu in the whole class!!!!!!
 *
 * cPar is an abstract base class for storing the values of module
 * (or channel) parameters. cPar supports several data types via subclasses:
 * cLongPar, cDoublePar, cBoolPar, cStringPar, cXMLPar.
 *
 * Data types are identified by type characters, returned by type():
 * <b>B</b> bool, <b>L</b> long, <b>D</b> double, <b>S</b> string, <b>X</b> XML. FIXME use enum!
 *
 * @ingroup SimCore
 */
class SIM_API cPar : public cObject
{
  protected:
    cProperties *props;   // if NULL, use the one from the component declaration

    // Flags are stored in cObject "flags" field. Values should be chosen so that
    // there is no collision with bits used by cObject.
    enum {
      FL_ISVOLATILE = 4,  // whether it was declared as "volatile" in NED
      FL_ISSET = 8,       // whether value is set (corresponds to old "input" flag)
      FL_HASDEFAULT = 16, // whether it has a default value
      FL_ISEXPR = 32,     // whether it stores a constant or an expression
      FL_HASCHANGED = 64  // whether it has changed since last asked
    };

  public:
    // parameter type
    enum Type {
        BOOL = 'B',
        DOUBLE = 'D',
        LONG = 'L',
        STRING = 'S',
        XML = 'X'
    };

  public:
    // internal: sets the ISVOLATILE flag; NOTE: may be necessary to invoke convertToConst() as well!
    virtual void setIsVolatile(bool f) {if (f) flags|=FL_ISVOLATILE; else flags&=~FL_ISVOLATILE;}

    // internal: mark value as unset; the current value (if set) becomes the default value
    virtual void markAsUnset();

    // internal: if !ISSET and has a default value, set it to the default;
    // do nothing if there's no default value
    virtual void applyDefaultValue();

    // internal: called as part of read(): gets the value from omnetpp.ini or from the user.
    virtual void doReadValue();

    // internal: sets owner component (module/channel) and index of this param in it
    cComponent *ownerComponent();

    // internal: create a parameter object representing the given type
    static cPar *createWithType(Type type);

  protected:
    /** @name Event hooks */
    //@{

    /**
     * Called each time before the value of this object changes.
     * This method can be used for tracking parameter changes.
     * This default implementation does nothing.
     */
    virtual void beforeChange();

    /**
     * Called each time after the value of this object changed. Changes in
     * flags don't count, only the (long, double, etc) value.
     * This method can be used for tracking parameter changes.
     */
    virtual void afterChange();
    //@}

  public:
    /** @name Constructors, destructor, assignment. */
    //@{

    /**
     * Constructor.
     */
    explicit cPar();

    /**
     * Copy constructor.
     */
    cPar(const cPar& other);

    /**
     * Destructor.
     */
    virtual ~cPar();

    /**
     * Assignment operator.
     */
    cPar& operator=(const cPar& otherpar);
    //@}

    /** @name Redefined cObject functions */
    //@{
    /**
     * Redefined change return type to cPar.
     */
    virtual cPar *dup() const;

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
     * Returns type character: 'B', 'D', 'L', 'S' or 'X'. // FIXME turn this into enum!!!!!
     */
    virtual char type() const = 0;  //FIXME use cPar::Type enum

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
     * Returns true if the parameter is not set. (It may still have a default
     * value though).
     */
    virtual bool isSet() const  {return flags & FL_ISSET;}

    /**
     * Returns true if the parameter has a default value.
     */
    virtual bool hasDefaultValue() const  {return flags & FL_HASDEFAULT;}

    /**
     * Returns true if the value has changed since the last changed() call.
     * Side effect: clears the 'changed' flag, so a next call will return
     * false.
     */
    virtual bool changed();

    /**
     * Return the properties for this parameter. See also unlockProperties().
     */
    virtual cProperties *properties() const;

    /**
     * Allows modification of parameter properties. By default, properties are
     * locked against modifications, because properties() returns a shared copy.
     * This method creates an own, modifiable copy for this parameter instance.
     */
    virtual cProperties *unlockProperties();
    //@}

    /** @name Setter functions. Note that overloaded assignment operators also exist. */
    //@{

    /**
     * Sets the value to the given bool value.
     */
    virtual cPar& setBoolValue(bool b) = 0;

    /**
     * Sets the value to the given long value.
     */
    virtual cPar& setLongValue(long l) = 0;

    /**
     * Sets the value to the given double value.
     */
    virtual cPar& setDoubleValue(double d) = 0;

    /**
     * Sets the value to the given string value.
     * The cPar will make its own copy of the string. NULL is also accepted
     * and treated as an empty string.
     */
    virtual cPar& setStringValue(const char *s) = 0;

    /**
     * Sets the value to the given string value.
     */
    virtual cPar& setStringValue(const std::string& s)  {setStringValue(s.c_str()); return *this;}

    /**
     * Sets the value to the given cXMLElement.
     */
    virtual cPar& setXMLValue(cXMLElement *node) = 0;

    /**
     * Sets the value to the given expression. This object will assume
     * the responsibility to delete the expression object.
     *
     * Note: if the parameter is marked as non-volatile (isVolatile()==false),
     * one should not set an expression as value. This is not enforced
     * by cPar though.
     */
    virtual cPar& setExpression(cExpression *e) = 0;
    //@}

    /** @name Getter functions. Note that overloaded conversion operators also exist. */
    //@{

    /**
     * Returns value as a boolean. The cPar type must be bool (B) or a numeric type.
     */
    virtual bool boolValue() const = 0;

    /**
     * Returns value as long. The cPar type must be types long (L),
     * double (D), Boolean (B), function (F), distribution (T),
     * compiled expression (C) or expression (X).
     */
    virtual long longValue() const = 0;

    /**
     * Returns value as double. The cPar type must be types long (L),
     * double (D), function (F), Boolean (B), distribution (T),
     * compiled expression (C) or expression (X).
     */
    virtual double doubleValue() const = 0;

    /**
     * Returns value as const char *. Only for string (S) type.
     */
    virtual std::string stringValue() const = 0;

    /**
     * Returns value as pointer to cXMLElement. The cPar type must be XML (M).
     */
    virtual cXMLElement *xmlValue() const = 0;

    /**
     * Returns pointer to the expression stored by the object, or NULL.
     */
    virtual cExpression *expression() const = 0;
    //@}

    /** @name Miscellaneous utility functions. */
    //@{

    /**
     * This method does the final touches on the parameter. It is invoked
     * at some point on all parameter objects before we start the simulation.
     *
     * - if the parameter is not set, gets the value from omnetpp.ini or
     *   interactively from the user, or sets the default value.
     *
     * - if the parameter is non-volatile, (isVolatile()==false), converts
     *   possible expression value to a constant (see convertToConst()).
     *
     * - if the parameter is volatile but contains "const" subexpressions,
     *   these parts are converted to a constant value.
     */
    virtual void read();

    /**
     * Replaces for non-const values, replaces the stored expression with its
     * evaluation.
     */
    virtual void convertToConst() = 0;

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

    /** @name Overloaded assignment and conversion operators. */
    //@{

    /**
     * Equivalent to setBoolValue().
     */
    cPar& operator=(bool b)  {return setBoolValue(b);}

    /**
     * Converts the argument to long, and calls setLongValue().
     */
    cPar& operator=(char c)  {return setLongValue((long)c);}

    /**
     * Converts the argument to long, and calls setLongValue().
     */
    cPar& operator=(unsigned char c)  {return setLongValue((long)c);}

    /**
     * Converts the argument to long, and calls setLongValue().
     */
    cPar& operator=(int i)  {return setLongValue((long)i);}

    /**
     * Converts the argument to long, and calls setLongValue().
     */
    cPar& operator=(unsigned int i)  {return setLongValue((long)i);}

    /**
     * Converts the argument to long, and calls setLongValue().
     */
    cPar& operator=(short i)  {return setLongValue((long)i);}

    /**
     * Converts the argument to long, and calls setLongValue().
     */
    cPar& operator=(unsigned short i)  {return setLongValue((long)i);}

    /**
     * Equivalent to setLongValue().
     */
    cPar& operator=(long l)  {return setLongValue(l);}

    /**
     * Converts the argument to long, and calls setLongValue().
     */
    cPar& operator=(unsigned long l) {return setLongValue((long)l);}

    /**
     * Equivalent to setDoubleValue().
     */
    cPar& operator=(double d)  {return setDoubleValue(d);}

    /**
     * Converts the argument to double, and calls setDoubleValue().
     */
    cPar& operator=(long double d)  {return setDoubleValue((double)d);}

    /**
     * Equivalent to setStringValue().
     */
    cPar& operator=(const char *s)  {return setStringValue(s);}

    /**
     * Equivalent to setStringValue().
     */
    cPar& operator=(const std::string& s)  {return setStringValue(s);}

    /**
     * Equivalent to setXMLValue().
     */
    cPar& operator=(cXMLElement *node)  {return setXMLValue(node);}

    /**
     * Equivalent to boolValue().
     */
    operator bool() const  {return boolValue();}

    /**
     * Calls longValue() and converts the result to char.
     */
    operator char() const  {return (char)longValue();}

    /**
     * Calls longValue() and converts the result to unsigned char.
     */
    operator unsigned char() const  {return (unsigned char)longValue();}

    /**
     * Calls longValue() and converts the result to int.
     */
    operator int() const  {return (int)longValue();}

    /**
     * Calls longValue() and converts the result to unsigned int.
     */
    operator unsigned int() const  {return (unsigned int)longValue();}

    /**
     * Calls longValue() and converts the result to short.
     */
    operator short() const  {return (short)longValue();}

    /**
     * Calls longValue() and converts the result to unsigned short.
     */
    operator unsigned short() const  {return (unsigned short)longValue();}

    /**
     * Equivalent to longValue().
     */
    operator long() const  {return longValue();}

    /**
     * Calls longValue() and converts the result to unsigned long.
     */
    operator unsigned long() const  {return longValue();}

    /**
     * Equivalent to doubleValue().
     */
    operator double() const  {return doubleValue();}

    /**
     * Calls doubleValue() and converts the result to long double.
     */
    operator long double() const  {return doubleValue();}

    /**
     * Equivalent to stringValue().
     */
    operator std::string() const  {return stringValue();}

    /**
     * Equivalent to stringValue(), but may can only be invoked when the
     * parameter's value is a string constant, and not the result of expression
     * evaluation. (In the latter case, an error is thrown.) This practically
     * means that this method cannot be applied on parameters declared as
     * "volatile" in NED -- for those you have to use <tt>stringValue().c_str()</tt>.
     */
    operator const char *() const;

    /**
     * Equivalent to xmlValue().
     */
    operator cXMLElement *() const  {return xmlValue();}

    //@}

    /** @name Compare functions */
    //@{

    /**
     * Evaluates both expressions and compares the resulting values.
     * If either of the objects is not set (isSet()==false) or they are
     * of different type(), false is returned.
     */
    virtual bool equals(cPar& other);

    /**
     * Compares two cPars by their value if they are numeric.
     * This function can be used to sort cPar objects in a priority
     * queue.
     */
    static int cmpbyvalue(cPar *one, cPar *other);
    //@}
};

#endif


