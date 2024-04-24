//==========================================================================
//   CPARIMPL.H  - part of
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

#ifndef __OMNETPP_CPARIMPL_H
#define __OMNETPP_CPARIMPL_H

#include "cpar.h"
#include "cexpression.h"
#include "cexception.h"
#include "fileline.h"
#include "opp_pooledstring.h"

namespace omnetpp {

class cExpression;
class cXMLElement;
class cProperties;
class cComponent;

namespace internal {

/**
 * @brief Internal class that stores values for cPar objects.
 *
 * cPar delegates almost all methods to cParImpl. Delegation is used to conserve
 * memory by using shared storage for module parameters that have the same values.
 *
 * cParImpl is an abstract base class, which supports several data types via
 * subclasses: cIntParImpl, cDoubleParImpl, cBoolParImpl, cStringParImpl,
 * cObjectParImpl, cXMLParImpl.
 *
 * @ingroup Internals
 */
class SIM_API cParImpl : public cNamedObject
{
  protected:
    // various flags, stored in cNamedObject::flags
    enum {
      FL_ISVOLATILE = 4,  // whether it was declared as "volatile" in NED
      FL_ISEXPR = 8,      // whether it stores a constant or an expression
      FL_ISSHARED = 16,   // used by cPar only: whether this object is shared among multiple cPars
      FL_CONTAINSVALUE = 32, // whether it has a value
      FL_ISSET = 64,      // whether the contained value is just a default value (false) or the set value (true)
      FL_ISMUTABLE = 128  // whether it was declared as "mutable" in NED
    };

  private:
    // unit (s, mW, GHz, baud, etc); optional
    opp_staticpooledstring unit = nullptr;

    // base directory for interpreting relative path names in the expression (e.g. xmldoc())
    opp_staticpooledstring baseDirectory = nullptr;

    // the file/line location where the value was assigned from
    FileLine sourceLoc;

    // global variables for statistics
    static long totalParimplObjs;
    static long liveParimplObjs;

  private:
    void copy(const cParImpl& other);

  public:
    typedef cPar::Type Type;

  protected:
    cValue evaluate(cExpression *expr, cComponent *context) const;
    void deleteOld(cExpression *expr);

  public:
    /** @name Constructors, destructor, assignment. */
    //@{

    /**
     * Constructor.
     */
    explicit cParImpl();

    /**
     * Copy constructor.
     */
    cParImpl(const cParImpl& other) : cNamedObject(other) {copy(other);}

    /**
     * Destructor.
     */
    virtual ~cParImpl();

    /**
     * Assignment operator.
     */
    cParImpl& operator=(const cParImpl& otherpar);
    //@}

    /** @name Redefined cObject member functions */
    //@{
    /**
     * Redefined change return type to cParImpl.
     */
    virtual cParImpl *dup() const override;

    /**
     * Redefined to throw a "packing not supported" exception.
     */
    virtual void parsimPack(cCommBuffer *buffer) const override;

    /**
     * Redefined to throw a "packing not supported" exception.
     */
    virtual void parsimUnpack(cCommBuffer *buffer) override;
    //@}

    /** @name Owner component, type, flags. */
    //@{
    /**
     * Returns the parameter type.
     */
    virtual Type getType() const = 0;

    /**
     * Returns true if the stored value is of a numeric type.
     */
    virtual bool isNumeric() const = 0;

    /**
     * Returns true if this parameter is marked in the NED file as "volatile".
     */
    virtual bool isVolatile() const {return flags & FL_ISVOLATILE;}

    /**
     * Returns true if this parameter is marked in the NED file as mutable.
     */
    virtual bool isMutable() const {return flags & FL_ISMUTABLE;}

    /**
     * Returns false if the stored value is a constant, and true if it is
     * an expression. (It is not examined whether the expression yields
     * a constant value.)
     */
    virtual bool isExpression() const {return flags & FL_ISEXPR;}

    /**
     * Used by cPar only: Returns true if this object is shared among multiple cPars.
     */
    virtual bool isShared() const {return flags & FL_ISSHARED;}

    /**
     * Returns true if the parameter contains a value. Note that isSet()
     * may still return true or false.
     */
    virtual bool containsValue() const  {return flags & FL_CONTAINSVALUE;}

    /**
     * Used by cPar only: Returns true if the parameter value is set,
     * false otherwise (i.e. if the object contains no value
     * or the current value is just a default).
     */
    virtual bool isSet() const {return flags & FL_ISSET;}

    /**
     * Sets the isVolatile flag. 
     * 
     * Note: It may be necessary to invoke
     * convertToConst(cComponent *context) as well.
     */
    virtual void setIsVolatile(bool f) {setFlag(FL_ISVOLATILE,f);}

    /**
     * Sets the isMutable flag.
     */
    virtual void setIsMutable(bool f) {setFlag(FL_ISMUTABLE,f);}

    /**
     * Sets the isShared flag.
     */
    virtual void setIsShared(bool f) {setFlag(FL_ISSHARED,f);}

    /**
     * Sets the isSet flag.
     */
    virtual void setIsSet(bool f) {setFlag(FL_ISSET,f);}

    /**
     * Returns the parameter's unit ("s", "mW", "Hz", "bps", etc),
     * as declared in the @unit property of the parameter in NED.
     * Unit is only meaningful with numeric parameters.
     */
    virtual const char *getUnit() const {return unit.c_str();}

    /**
     * Initialize the parameter's unit (normally from the @unit property).
     */
    virtual void setUnit(const char *s) {unit = s;}

    /**
     * Returns the base directory for interpreting relative path names
     * in the expression.
     */
    virtual const char *getBaseDirectory() const {return baseDirectory.c_str();}

    /**
     * Sets the base directory for interpreting relative path names in the
     * expression (e.g. in xmldoc()).
     */
    virtual void setBaseDirectory(const char *s) {baseDirectory = s;}

    /**
     * Sets the value returned by getSourceLocation().
     */
    virtual void setSourceLocation(const FileLine &loc);

    /**
     * Clears the value returned by getSourceLocation().
     */
    virtual void clearSourceLocation();

    /**
     * Returns the file:line info where this parameter was parsed from.
     * Returns a blank object if no such info is available.
     */
    virtual const FileLine& getSourceLocation() const {return sourceLoc;}
    //@}

    /** @name Setter functions. Note that overloaded assignment operators also exist. */
    //@{

    /**
     * Sets the value to the given bool value.
     */
    virtual void setBoolValue(bool b) = 0;

    /**
     * Sets the value to the given integer value.
     */
    virtual void setIntValue(intval_t l) = 0;

    /**
     * Sets the value to the given double value.
     */
    virtual void setDoubleValue(double d) = 0;

    /**
     * Sets the value to the given string value.
     * The cParImpl will make its own copy of the string. nullptr is also accepted
     * and treated as an empty string.
     */
    virtual void setStringValue(const char *s) = 0;

    /**
     * Sets the value to the given string value.
     */
    virtual void setStringValue(const std::string& s)  {setStringValue(s.c_str());}

    /**
     * Sets the value to the given object.
     */
    virtual void setObjectValue(cObject *object) = 0;

    /**
     * Sets the value to the given cXMLElement.
     */
    virtual void setXMLValue(cXMLElement *node) = 0;

    /**
     * Sets the value to the given expression. This object will assume
     * the responsibility to delete the expression object.
     *
     * Note: If the parameter is marked as non-volatile (isVolatile()==false),
     * one should not set an expression as value. This is not enforced
     * by cParImpl though.
     */
    virtual void setExpression(cExpression *e) = 0;
    //@}

    /** @name Getter functions. */
    //@{

    /**
     * Returns value as a boolean. The cParImpl type must be BOOL.
     */
    virtual bool boolValue(cComponent *context) const = 0;

    /**
     * Returns value as an integer. The cParImpl type must be INT.
     * Note: Implicit conversion from DOUBLE is intentionally missing.
     */
    virtual intval_t intValue(cComponent *context) const = 0;

    /**
     * Returns value as a double. The cParImpl type must be DOUBLE.
     * Note: Implicit conversion from INT is intentionally missing.
     */
    virtual double doubleValue(cComponent *context) const = 0;

    /**
     * Returns value as const char *. The cParImpl type must be STRING.
     * This method may only be invoked when the parameter's value is a
     * string constant and not the result of expression evaluation, otherwise
     * an error is thrown. This practically means this method cannot be used
     * on parameters declared as "volatile string" in NED; they can only be
     * accessed using stdstringValue().
     */
    virtual const char *stringValue(cComponent *context) const = 0;

    /**
     * Returns value as string. The cParImpl type must be STRING.
     */
    virtual std::string stdstringValue(cComponent *context) const = 0;

    /**
     * Returns value as pointer to a cObject. The cParImpl type must be OBJECT.
     */
    virtual cObject *objectValue(cComponent *context) const = 0;

    /**
     * Returns value as pointer to cXMLElement. The cParImpl type must be XML.
     */
    virtual cXMLElement *xmlValue(cComponent *context) const = 0;

    /**
     * Returns pointer to the expression stored by the object, or nullptr.
     */
    virtual cExpression *getExpression() const = 0;
    //@}

    /** @name Miscellaneous utility functions. */
    //@{

    /**
     * If the parameter contains an expression, this method replaces the stored
     * expression with its evaluation. It preserves the stored file/line info,
     * which may or may not be what is intended.
     */
    virtual void convertToConst(cComponent *context) = 0;

    /**
     * Convert the value from string, and store the result.
     * If the text cannot be parsed, an exception is thrown, which
     * can be caught as std::runtime_error& if necessary.
     */
    virtual void parse(const char *text, FileLine loc) = 0;

    /**
     * Factory method: creates a parameter object representing the given type.
     */
    static cParImpl *createWithType(Type type);

    /**
     * Compares two cParImpls, including name, type, flags, stored value or expression.
     * Makes it possible to use cParImpl as a key in std::map or std::set.
     */
    virtual int compare(const cParImpl *other) const;

    /**
     * Needed for cPar's forEachChild().
     */
    virtual void forEachChild(cVisitor *v, cComponent *context) {}

    using cObject::forEachChild;
    //@}

    /** @name Statistics. */
    //@{
    /**
     * Returns the total number of objects created since the start of the program
     * (or since the last reset). The counter is incremented by cOwnedObject constructor.
     * Counter is `signed` to make it easier to detect if it overflows
     * during very long simulation runs.
     * May be useful for profiling or debugging memory leaks.
     */
    static long getTotalParImplObjectCount() {return totalParimplObjs;}

    /**
     * Returns the number of objects that currently exist in the program.
     * The counter is incremented by cOwnedObject constructor and decremented by
     * the destructor.
     * May be useful for profiling or debugging memory leaks.
     */
    static long getLiveParImplObjectCount() {return liveParimplObjs;}

    /**
     * Reset counters used by getTotalObjectCount() and getLiveObjectCount().
     * (Note that getLiveObjectCount() may go negative after a reset call.)
     */
    static void resetParImplObjectCounters()  {totalParimplObjs=liveParimplObjs=0L;}
    //@}
};

}  // namespace internal
}  // namespace omnetpp


#endif


