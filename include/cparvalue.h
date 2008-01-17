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
#include "cstringpool.h"
#include "cexception.h"

NAMESPACE_BEGIN

class cExpression;
class cXMLElement;
class cProperties;
class cComponent;


/**
 * Internal class that stores parameter values. cPar delegates almost all
 * methods to cParValue. Delegation was introduced to save memory by using
 * shared storage for module parameters of the same values.
 *
 * cParValue is an abstract base class, which supports several data types via
 * subclasses: cLongPar, cDoublePar, cBoolPar, cStringPar, cXMLPar.
 *
 * @ingroup Internals
 */
class SIM_API cParValue : public cNamedObject
{
  protected:
    // various flags, stored in cNamedObject::flags
    enum {
      FL_ISVOLATILE = 4,  // whether it was declared as "volatile" in NED
      FL_HASVALUE = 8,    // whether it has a value
      FL_ISEXPR = 16,     // whether it stores a constant or an expression
      FL_ISSHARED = 32,   // used by cPar only: whether this object is shared among multiple cPars
      FL_ISINPUT = 64     // used by cPar only: whether this is just a default value and real value will have to be asked from user or read from omnetpp.ini
    };

  private:
    // unit (s, mW, GHz, baud, etc); optional
    const char *unitp; // stringpooled

    // global variables for statistics
    static long total_parvalue_objs;
    static long live_parvalue_objs;
    static cStringPool unitStringPool;

  public:
    typedef cPar::Type Type;

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

    /** @name Redefined cObject member functions */
    //@{
    /**
     * Returns a one-line description of the object.
     * @see detailedInfo()
     */
    virtual std::string info() const;

    /**
     * Returns a long description of the object.
     */
    virtual std::string detailedInfo() const;

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
     * Returns true if this parameter is marked in the NED file as "volatile".
     */
    virtual bool isVolatile() const {return flags & FL_ISVOLATILE;}

    /**
     * Returns false if the stored value is a constant, and true if it is
     * an expression. (It is not examined whether the expression yields
     * a constant value.)
     */
    virtual bool isExpression() const {return flags & FL_ISEXPR;}

    /**
     * Returns true if the parameter has a value set.
     */
    virtual bool hasValue() const  {return flags & FL_HASVALUE;}

    /**
     * Used by cPar only: Returns true if this object is shared among multiple cPars.
     */
    virtual bool isShared() const {return flags & FL_ISSHARED;}

    /**
     * Used by cPar only: whether this is just a default value and real value
     * will have to be asked from user or read from omnetpp.ini
     */
    virtual bool isInput() const {return flags & FL_ISINPUT;}

    /**
     * Sets the isVolatile flag. NOTE: It may be necessary to invoke
     * convertToConst(cComponent *context) as well.
     */
    virtual void setIsVolatile(bool f) {if (f) flags|=FL_ISVOLATILE; else flags&=~FL_ISVOLATILE;}

    /**
     * Sets the isShared flag.
     */
    virtual void setIsShared(bool f) {if (f) flags|=FL_ISSHARED; else flags&=~FL_ISSHARED;}

    /**
     * Sets the isInput flag.
     */
    virtual void setIsInput(bool f) {if (f) flags|=FL_ISINPUT; else flags&=~FL_ISINPUT;}

    /**
     * Returns the parameter value's unit ("s", "mW", "Hz", "bps", etc).
     * This normally comes from the assignment, possibly after performing
     * conversion ("64kbps" get stored as d=64000, unit="bps") and must match
     * the @unit property of the parameter, declared in NED. Unit is only
     * meaningful with numeric parameters.
     */
    virtual const char *unit() const;

    /**
     * Sets the parameter value's unit ("s", "mW", "Hz", "bps", etc).
     */
    virtual void setUnit(const char *s);
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

    /** @name Getter functions. */
    //@{

    /**
     * Returns value as a boolean. The cParValue type must be BOOL.
     */
    virtual bool boolValue(cComponent *context) const = 0;

    /**
     * Returns value as long. The cParValue type must be LONG or DOUBLE.
     */
    virtual long longValue(cComponent *context) const = 0;

    /**
     * Returns value as long. The cParValue type must be LONG or DOUBLE.
     */
    virtual double doubleValue(cComponent *context) const = 0;

    /**
     * Returns value as const char *. Only for STRING type.
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
     * Returns value as pointer to cXMLElement. The cParValue type must be XML.
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
     * Returns true if this expression contains const subexpressions.
     */
    virtual bool containsConstSubexpressions() const {return false;} //FIXME TBD

    /**
     * Evaluates const subexpressions, and replaces them with their values.
     * See cDynamicExpression::Elem::CONSTSUBEXPR.
     */
    virtual void evaluateConstSubexpressions(cComponent *context) {} //FIXME TBD

    /**
     * Returns the value in text form.
     */
    virtual std::string toString() const = 0;

    /**
     * Convert the value from string, and store the result.
     * If the text cannot be parsed, an exception is thrown, which
     * can be caught as std::runtime_error& if necessary.
     */
    virtual void parse(const char *text) = 0;

    /**
     * Factory method: creates a parameter object representing the given type.
     */
    static cParValue *createWithType(Type type);
    //@}

    /** @name Compare functions */
    //@{

    /**
     * Compares two cParValues, including name, type, flags, stored value or expression.
     * Makes it possible to use cParValue as a key in std::map or std::set.
     */
    virtual int compare(const cParValue *other) const;
    //@}

    /** @name Statistics. */
    //@{
    /**
     * Returns the total number of objects created since the start of the program
     * (or since the last reset). The counter is incremented by cOwnedObject constructor.
     * Counter is <tt>signed</tt> to make it easier to detect if it overflows
     * during very long simulation runs.
     * May be useful for profiling or debugging memory leaks.
     */
    static long totalParValueObjectCount() {return total_parvalue_objs;}

    /**
     * Returns the number of objects that currently exist in the program.
     * The counter is incremented by cOwnedObject constructor and decremented by
     * the destructor.
     * May be useful for profiling or debugging memory leaks.
     */
    static long liveParValueObjectCount() {return live_parvalue_objs;}

    /**
     * Reset counters used by totalObjectCount() and liveObjectCount().
     * (Note that liveObjectCount() may go negative after a reset call.)
     */
    static void resetParValueObjectCounters()  {total_parvalue_objs=live_parvalue_objs=0L;}
    //@}
};

NAMESPACE_END


#endif


