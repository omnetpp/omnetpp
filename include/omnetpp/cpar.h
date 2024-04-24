//==========================================================================
//   CPAR.H  - part of
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

#ifndef __OMNETPP_CPAR_H
#define __OMNETPP_CPAR_H

#include "cownedobject.h"
#include "cexpression.h"
#include "cexception.h"
#include "fileline.h"

namespace omnetpp {

class cExpression;
class cXMLElement;
class cProperties;
class cComponent;
class cValue;

namespace internal { class cParImpl; }

/**
 * @brief Represents a module or channel parameter.
 *
 * When a module or channel is created, parameter objects are added
 * automatically, based on the NED declaration of the module/channel.
 * It is not possible to create further parameters (or to remove parameters)
 * at runtime. This is enforced by the cPar constructor being private.
 *
 * Parameters get their initial values automatically, from the NED
 * declarations and the configuration. It is possible to change the
 * parameter value during runtime (see various setter methods and
 * operator='s), but not the type of the parameter (see getType()).
 * The type correspond to NED types (bool, double, int, string, xml),
 * and cannot be changed at runtime.
 *
 * The module or channel object can get notified when a parameter is
 * changed; one has to override cComponent::handleParameterChange()
 * for that.
 *
 * **Note:** In earlier versions of \opp, cPar could be used as a
 * general value storage object, and attached to cMessages as well.
 * From the 4.0 version, simulation models should use cMsgPar for that.
 *
 * **Implementation note:** from the 4.0 version, almost all methods
 * of cPar delegate to an internal cParImpl object, which actually stores
 * the value, and generally does the real job. This was done to allow
 * sharing parameter objects which have the same name, same value, etc.
 * among module/channel instances. This significantly reduces memory
 * consumption of most simulation models. Because cPar is just a thin
 * wrapper around cParImpl, cPar is not meant for subclassing, and
 * none if its methods are virtual. cParImpl and subclasses should
 * be regarded as internal data structures, and should not be
 * directly accessed from model code.
 *
 * @ingroup ModelComponents
 */
class SIM_API cPar : public cObject
{
    friend class cComponent;
  public:
    enum Type {
        BOOL = 'B',
        DOUBLE = 'D',
        INT = 'L',
        STRING = 'S',
        OBJECT = 'O',
        XML = 'X',
        LONG = INT // for backward compatibility
    };

  private:
    typedef internal::cParImpl cParImpl;
    cComponent *ownerComponent;
    cParImpl *p;
    cComponent *evalContext;

  private:
    // private constructor and destructor -- only cComponent is allowed to create parameters
    cPar() {ownerComponent = evalContext = nullptr; p = nullptr;}
    virtual ~cPar();
    // internal, called from cComponent
    void init(cComponent *ownercomponent, cParImpl *p);
    // internal
    void moveto(cPar& other);
    // internal: called each time before the value of this object changes.
    void beforeChange(bool isInternalChange=false);
    // internal: called each time after the value of this object changes.
    void afterChange();
    // internal: replace expression with the value it evaluates to
    void doConvertToConst(bool isInternalChange=true);


  public:
    // internal, used by cComponent::finalizeParameters()
    void read();
    // internal, used by cComponent::finalizeParameters()
    void finalize();
    // internal: applies the default value if there is one
    void acceptDefault();
    // internal
    void setImpl(cParImpl *p);
    // internal
    cParImpl *impl() const {return p;}
    // internal
    cParImpl *copyIfShared();

#ifdef SIMFRONTEND_SUPPORT
    // internal
    virtual bool hasChangedSince(int64_t lastRefreshSerial);
#endif

  public:
    /** @name Redefined cObject methods */
    //@{
    /**
     * Assignment operator.
     */
    void operator=(const cPar& other);

    /**
     * Returns the parameter name.
     */
    virtual const char *getName() const override;

    /**
     * Returns the value in text form.
     */
    virtual std::string str() const override;

    /**
     * Returns the component (module/channel) this parameter belongs to.
     * Note: return type is cObject only for technical reasons, it can be
     * safely cast to cComponent. Consider using getOwnerComponent() instead.
     */
    virtual cObject *getOwner() const override; // note: cannot return cComponent* (covariant return type) due to declaration order

    /**
     * Calls v->visit(this) for contained objects.
     * See cObject for more details.
     */
    virtual void forEachChild(cVisitor *v) override;
    //@}

    /** @name Type, flags. */
    //@{
    /**
     * Returns the parameter type.
     */
    Type getType() const;

    /**
     * Returns the given type as a string.
     */
    static const char *getTypeName(Type t);

    /**
     * Returns true if the stored value is of a numeric type.
     */
    bool isNumeric() const;

    /**
     * Returns true if this parameter is marked in the NED file as "volatile".
     * This flag affects the operation of setExpression().
     */
    bool isVolatile() const;

    /**
     * Returns true if this parameter is marked in the NED file as mutable.
     * A non-mutable parameter cannot have its value changed.
     */
    bool isMutable() const;

    /**
     * Returns false if the stored value is a constant, and true if it is
     * an expression. (It is not examined whether the expression yields
     * a constant value.)
     */
    bool isExpression() const;

    /**
     * Returns true if the parameter value expression is shared among several
     * modules to save memory. This flag is purely informational, and whether
     * a parameter is shared or not does not affect operation at all.
     */
    bool isShared() const;

    /**
     * Returns true if the parameter is assigned a value, and false otherwise.
     * Parameters of an already initialized module or channel are guaranteed to
     * assigned, so this method will return true for them.
     */
    bool isSet() const;

    /**
     * Returns true if the parameter is set (see isSet()) or contains a default
     * value, and false otherwise. Parameters of an already initialized module or
     * channel are guaranteed to be assigned, so this method will return true for them.
     */
    bool containsValue() const;

    /**
     * Return the properties for this parameter. Properties cannot be changed
     * at runtime.
     */
    cProperties *getProperties() const;

    /**
     * Returns the component (module or channel) that owns this parameter object.
     */
    cComponent *getOwnerComponent() const { return ownerComponent;}

    /**
     * Returns the base directory for interpreting relative path names
     * in the contained expression.
     */
    virtual const char *getBaseDirectory() const;

    /**
     * Returns the file:line info where the value of this parameter was parsed from.
     * Returns empty string if no such info is available.
     */
    virtual std::string getSourceLocation() const;
    //@}

    /** @name Setter functions. Note that overloaded assignment operators also exist. */
    //@{

    /**
     * Sets the value to the given bool value.
     */
    cPar& setBoolValue(bool b);

    /**
     * Sets the value to the given integer value.
     */
    cPar& setIntValue(intval_t l);

    /**
     * Sets the value to the given double value.
     */
    cPar& setDoubleValue(double d);

    /**
     * Sets the value to the given string value.
     * The cPar will make its own copy of the string. nullptr is also accepted
     * and treated as an empty string.
     */
    cPar& setStringValue(const char *s);

    /**
     * Sets the value to the given string value.
     */
    cPar& setStringValue(const std::string& s)  {setStringValue(s.c_str()); return *this;}

    /**
     * Sets the value to the given object.
     */
    cPar& setObjectValue(cObject *object);

    /**
     * Sets the value to the given cXMLElement.
     */
    cPar& setXMLValue(cXMLElement *node);

    /**
     * Sets the parameter's value. This method invokes the value setter method
     * (setBoolValue(), setIntValue(), etc.) that corresponds to the parameter's
     * type with the value in the cValue object. If the value's type doesn't
     * match the parameter's type, an error is raised.
     */
    cPar& setValue(const cValue& value);

    /**
     * Sets the value to the given expression. This object will assume
     * the responsibility to delete the expression object.
     *
     * The evalcontext parameter determines the module or channel in the
     * context of which the expression will be evaluated. If evalcontext
     * is nullptr, the owner of this parameter will be used.
     *
     * Note: If the parameter is marked as non-volatile (isVolatile()==false),
     * one should not set an expression as value. This is not enforced
     * by cPar though.
     *
     * @see getOwner(), getEvaluationContext(), setEvaluationContext()
     */
    cPar& setExpression(cExpression *e, cComponent *evalcontext=nullptr);

    /**
     * If the parameter contains an expression (see isExpression()), this method
     * sets the evaluation context for the expression.
     *
     * @see getEvaluationContext(), isExpression(), setExpression()
     */
    void setEvaluationContext(cComponent *ctx)  {evalContext = ctx;}
    //@}

    /** @name Getter functions. Note that overloaded conversion operators also exist. */
    //@{

    /**
     * Returns value as a boolean. The cPar type must be BOOL.
     */
    bool boolValue() const;

    /**
     * Returns value as intval_t. The cPar type must be INT.
     * Note: Implicit conversion from DOUBLE is intentionally missing.
     */
    intval_t intValue() const;

    /**
     * Returns value as double. The cPar type must be DOUBLE.
     * Note: Implicit conversion from INT is intentionally missing.
     */
    double doubleValue() const;

    /**
     * Returns the numeric value as a double converted to the given unit.
     * If the current unit cannot be converted to the given one, an error
     * will be thrown. The type must be DOUBLE or INT.
     */
    double doubleValueInUnit(const char *targetUnit) const;

    /**
     * Returns the parameter's unit ("s", "mW", "Hz", "bps", etc) as declared
     * with the @unit property of the parameter in NED, or nullptr if no unit
     * was specified. Unit is only valid for INT and DOUBLE types.
     */
    const char *getUnit() const;

    /**
     * Returns value as const char *. The cPar type must be STRING.
     * This method may only be invoked when the parameter's value is a
     * string constant and not the result of expression evaluation, otherwise
     * an error is thrown. This practically means this method cannot be used
     * on parameters declared as "volatile string" in NED; they can only be
     * accessed using stdstringValue().
     */
    const char *stringValue() const;

    /**
     * Returns value as string. The cPar type must be STRING.
     */
    std::string stdstringValue() const;

    /**
     * Convenience method: returns true of the parameter contains an empty string.
     * The cPar type must be STRING.
     */
    bool isEmptyString() const {return stdstringValue().empty();}

    /**
     * Returns the value as object. The cPar type must be OBJECT. The ownership
     * of the object remains with the cPar. It is not possible for the user
     * to take ownership of the object, but creating a copy via dup() is an
     * option.
     *
     * Lifetime of the returned objects:
     *
     * The returned object pointer is stored within the parameter. A setObjectValue()
     * call will delete the previous object (given that the parameter owns the object).
     *
     * Note: If the object parameter refers to an object which it doesn't own,
     * it never deletes the object. If an object doesn't know its owner (it is
     * a cObject but not a cOwnedObject), it is considered owned.
     *
     * If the parameter is declared volatile (see isVolatile()), the returned
     * object pointer (the pointer of the object which was produced by evaluating
     * the expression) is stored. Upon the next objectValue() call, the
     * previously returned object is deleted (given that the parameter owns the
     * object), and replaced with the new object.
     *
     * Note that parameters may be shared under the hood. This means that a
     * previously returned object may also be deleted if another module's
     * similar volatile parameter is evaluated using objectValue().
     *
     * Therefore, the lifetime of returned objects should be considered very
     * limited. In simple terms, the object should either be used only locally
     * and then its pointer forgotten, or be cloned using dup() for long-term use.
     */
    cObject *objectValue() const;

    /**
     * Returns value as pointer to cXMLElement. The cPar type must be XML.
     *
     * The lifetime of the returned object tree is undefined, but it is
     * valid at least until the end of the current simulation event or
     * initialize() call. Modules are expected to process their XML
     * configurations at once (within one event or within initialize()),
     * and not hang on to pointers returned from this method. The reason
     * for the limited lifetime is that this method may return pointers to
     * objects stored in an internal XML document cache, and the simulation
     * kernel reserves the right to discard cached XML documents at any time
     * to free up memory, and re-load them on demand (i.e. when xmlValue() is
     * called again).
     */
    cXMLElement *xmlValue() const;

    /**
     * Generic access to the parameter's value. This method delegates to the
     * value accessor method (boolValue(), intValue(), etc.) that corresponds
     * to the parameter's type, and returns the result.
     */
    cValue getValue() const;

    /**
     * Returns pointer to the expression stored by the object, or nullptr.
     */
    cExpression *getExpression() const;

    /**
     * If the parameter contains an expression, this method returns the
     * module or channel in the context of which the expression will be
     * evaluated. (The context affects the resolution of parameter
     * references, and NED operators like `index` or `sizeof()`.)
     * If the parameter does not contain an expression, the return value is
     * undefined.
     *
     * @see isExpression(), setEvaluationContext()
     */
    cComponent *getEvaluationContext() const  {return evalContext;}
    //@}

    /** @name Miscellaneous utility functions. */
    //@{
    /**
     * For non-const values, replaces the stored expression with its
     * evaluation.
     */
    void convertToConst() {doConvertToConst(false);}

    /**
     * Converts the value from string, and stores the result.
     * If the text cannot be parsed, an exception is thrown, which
     * can be caught as std::runtime_error& if necessary.
     *
     * Note: This method understands expressions too, but does NOT handle
     * the special values "default" and "ask".
     */
    void parse(const char *text, const char *baseDirectory=nullptr, FileLine loc=FileLine(), bool resetEvalContext=true);
    //@}

    /** @name Overloaded assignment and conversion operators. */
    //@{

    /**
     * Equivalent to setBoolValue().
     */
    cPar& operator=(bool b)  {return setBoolValue(b);}

    /**
     * Delegates to setIntValue().
     */
    cPar& operator=(char c)  {return setIntValue(c);}

    /**
     * Delegates to setIntValue().
     */
    cPar& operator=(unsigned char c)  {return setIntValue(c);}

    /**
     * Delegates to setIntValue().
     */
    cPar& operator=(int i)  {return setIntValue(i);}

    /**
     * Delegates to setIntValue().
     */
    cPar& operator=(unsigned int i)  {return setIntValue(i);}

    /**
     * Delegates to setIntValue().
     */
    cPar& operator=(short i)  {return setIntValue(i);}

    /**
     * Delegates to setIntValue().
     */
    cPar& operator=(unsigned short i)  {return setIntValue(i);}

    /**
     * Equivalent to setIntValue().
     */
    cPar& operator=(long i)  {return setIntValue(i);}

    /**
     * Delegates to setIntValue().
     */
    cPar& operator=(unsigned long i) {return setIntValue(checked_int_cast<intval_t>(i, this));}

    /**
     * Delegates to setIntValue().
     */
    cPar& operator=(long long i)  {return setIntValue(checked_int_cast<intval_t>(i, this));}

    /**
     * Delegates to setIntValue().
     */
    cPar& operator=(unsigned long long i)  {return setIntValue(checked_int_cast<intval_t>(i, this));}

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
     * Equivalent to setObjectValue().
     */
    cPar& operator=(cObject *object)  {return setObjectValue(object);}

    /**
     * Equivalent to setXMLValue().
     */
    cPar& operator=(cXMLElement *node)  {return setXMLValue(node);}

    /**
     * Equivalent to boolValue().
     */
    operator bool() const  {return boolValue();}

    /**
     * Calls intValue() and converts the result to char.
     * An exception is thrown if the conversion would result in a data loss,
     */
    operator char() const  {return checked_int_cast<char>(intValue(), this);}

    /**
     * Calls intValue() and converts the result to unsigned char.
     * An exception is thrown if the conversion would result in a data loss,
     */
    operator unsigned char() const  {return checked_int_cast<unsigned char>(intValue(), this);}

    /**
     * Calls intValue() and converts the result to int.
     * An exception is thrown if the conversion would result in a data loss,
     */
    operator int() const  {return checked_int_cast<int>(intValue(), this);}

    /**
     * Calls intValue() and converts the result to unsigned int.
     * An exception is thrown if the conversion would result in a data loss,
     */
    operator unsigned int() const  {return checked_int_cast<unsigned int>(intValue(), this);}

    /**
     * Calls intValue() and converts the result to short.
     * An exception is thrown if the conversion would result in a data loss,
     */
    operator short() const  {return checked_int_cast<short>(intValue(), this);}

    /**
     * Calls intValue() and converts the result to unsigned short.
     * An exception is thrown if the conversion would result in a data loss,
     */
    operator unsigned short() const  {return checked_int_cast<unsigned short>(intValue(), this);}

    /**
     * Calls intValue() and converts the result to long.
     * An exception is thrown if the conversion would result in a data loss,
     */
    operator long() const  {return checked_int_cast<long>(intValue(), this);}

    /**
     * Calls intValue() and converts the result to unsigned long.
     * An exception is thrown if the conversion would result in a data loss,
     */
    operator unsigned long() const  {return checked_int_cast<unsigned long>(intValue(), this);}

    /**
     * Calls intValue() and converts the result to long long.
     * An exception is thrown if the conversion would result in a data loss,
     */
    operator long long() const  {return checked_int_cast<long long>(intValue(), this);}

    /**
     * Calls intValue() and converts the result to unsigned long long.
     * An exception is thrown if the conversion would result in a data loss,
     */
    operator unsigned long long() const  {return checked_int_cast<unsigned long long>(intValue(), this);}

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
    operator const char *() const  {return stringValue();}

    /**
     * Equivalent to stdstringValue().
     */
    operator std::string() const  {return stdstringValue();}

    /**
     * Equivalent to objectValue().
     */
    operator cObject *() const  {return objectValue();}

    /**
     * Equivalent to xmlValue(). 
     * 
     * Note: The lifetime of the returned object tree
     * is limited; see xmlValue() for details.
     */
    operator cXMLElement *() const  {return xmlValue();}
    //@}
};

}  // namespace omnetpp


#endif




