//==========================================================================
//   CVALUE.H  - part of
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

#ifndef __OMNETPP_CVALUE_H
#define __OMNETPP_CVALUE_H

#include <string>
#include "simkerneldefs.h"
#include "cexception.h"
#include "opp_string.h"
#include "opp_pooledstring.h"
#include "any_ptr.h"
#include "simutil.h"

namespace omnetpp {

class cPar;
class cXMLElement;
class cDynamicExpression;

/**
 * @brief A variant-like value class used during evaluating NED expressions.
 *
 * See notes below.
 *
 * **Pointer values**
 *
 * With type==POINTER, cValue only stored the pointer, and does nothing extra
 * on top of that. If the pointer points to an object, the object's ownership
 * is unaffected, and the object is never deleted or cloned by cValue.
 *
 * @see cDynamicExpression, cNedFunction, Define_NED_Function()
 * @ingroup Expressions
 */
class SIM_API cValue
{
    friend class cDynamicExpression;
  public:
    /**
     * Type of the value stored in a cValue object.
     */
    // Note: char codes need to be present and be consistent with cNedFunction::getArgTypes()!
    enum Type {
        UNDEF = 0,
        BOOL = 'B',
        INT = 'L',
        DOUBLE = 'D',
        STRING = 'S',
        POINTER = 'O',
        OBJECT OPP_DEPRECATED_ENUMERATOR("and use cValue::containsObject()") = POINTER,
        XML OPP_DEPRECATED_ENUMERATOR("and use cValue::containsXml()") = POINTER,
        DBL OPP_DEPRECATED_ENUMERATOR("renamed to DOUBLE") = DOUBLE,
        STR OPP_DEPRECATED_ENUMERATOR("renamed to STRING") = STRING
    };
    Type type;

  private:
    bool bl;
    intval_t intv;
    double dbl;
    opp_staticpooledstring unit = nullptr; // for INT/DOUBLE; may be nullptr
    std::string s;
    any_ptr ptr;
    static const char *OVERFLOW_MSG;

  private:
#ifdef NDEBUG
    void assertType(Type) const {}
#else
    void assertType(Type t) const {if (type!=t) cannotCastError(t);}
#endif
    [[noreturn]] void cannotCastError(Type t) const;
  public:
    // internal, for inspectors only:
    static cObject *getContainedObject(const cValue *p);

  public:
    /** @name Constructors */
    //@{
    cValue()  {type=UNDEF;}
    cValue(bool b)  {set(b);}
    cValue(int l)  {set((intval_t)l);}
    cValue(int l, const char *unit)  {set((intval_t)l, unit);}
    cValue(intval_t l)  {set(l);}
    cValue(intval_t l, const char *unit)  {set(l, unit);}
    cValue(double d)  {set(d);}
    cValue(double d, const char *unit)  {set(d,unit);}
    cValue(const char *s)  {set(s);}
    cValue(const std::string& s)  {set(s);}
    cValue(const opp_string& s)  {set(s);}
    cValue(any_ptr ptr)  {set(ptr);}
    cValue(cObject *obj)  {set(obj);}
    cValue(const void *) = delete; // prevent non-cObject pointers from silently being converted to bool
    cValue(const cValue&) = default;
    //@}

    /**
     * Assignment
     */
    void operator=(const cValue& other);

    /** @name Type, unit conversion and misc. */
    //@{
    /**
     * Returns the value type.
     */
    Type getType() const  {return type;}

    /**
     * Returns the value type as string.
     */
    const char *getTypeName() const  {return getTypeName(type);}

    /**
     * Returns the given type as a string.
     */
    static const char *getTypeName(Type t);

    /**
     * Returns true if the stored value is of a numeric type.
     */
    bool isNumeric() const {return type==DOUBLE || type==INT;}

    /**
     * Returns true if the value is not empty, i.e. type is not UNDEF.
     */
    bool isSet() const  {return type!=UNDEF;}

    /**
     * Returns the value in text form.
     */
    std::string str() const;

    /**
     * Comparison. Note that two cValues are equal if they are of the same type
     * (i.e. there is no implicit int <--> double conversion); if they contain
     * object pointers, they are equal if they point to the same object.
     */
    bool operator==(const cValue& other);

    /**
     * Convert the given number into the target unit (e.g. milliwatt to watt).
     * Throws an exception if conversion is not possible (unknown/unrelated units).
     *
     * @see convertTo(), doubleValueInUnit(), setUnit()
     */
    static double convertUnit(double d, const char *unit, const char *targetUnit);

    /**
     * Invokes parseQuantity(), and converts the result into the given unit.
     * If conversion is not possible (unrelated or unknown units), and error
     * is thrown.
     */
    static double parseQuantity(const char *str, const char *expectedUnit=nullptr);

    /**
     * Converts a quantity given as string to a double, and returns it, together
     * with the unit it was given in. If there are several numbers and units
     * (see syntax), everything is converted into the last unit.
     *
     * Syntax: `<number> | (<number> <unit>)+`
     *
     * If there is a syntax error, or if unit mismatch is found (i.e. distance
     * is given instead of time), the method throws an exception.
     */
    static double parseQuantity(const char *str, std::string& outActualUnit);
    //@}

    /** @name Setter functions. Note that overloaded assignment operators also exist. */
    //@{

    /**
     * Sets the value to the given bool value.
     */
    void set(bool b) {type=BOOL; bl=b;}

    /**
     * Sets the value to the given integer value and measurement unit (optional).
     * The unit string pointer is expected to stay valid during the entire
     * duration of the simulation (see related class comment).
     */
    void set(intval_t l, const char *unit=nullptr) {type=INT; intv=l; this->unit=unit;}

    /**
     * Sets the value to the given integer value and measurement unit (optional).
     * The unit string pointer is expected to stay valid during the entire
     * duration of the simulation (see related class comment).
     * This is a convenience method that delegates to the intval_t version.
     */
    void set(int l, const char *unit=nullptr) {set((intval_t)l, unit);}

    /**
     * Sets the value to the given double value and measurement unit (optional).
     * The unit string pointer is expected to stay valid during the entire
     * duration of the simulation (see related class comment).
     */
    void set(double d, const char *unit=nullptr) {type=DOUBLE; dbl=d; this->unit=unit;}

    /**
     * Sets the value to the given integer value, preserving the current
     * measurement unit. The object must already have the INT type.
     */
    void setPreservingUnit(intval_t l) {assertType(INT); intv=l;}

    /**
     * Sets the value to the given double value, preserving the current
     * measurement unit. The object must already have the DOUBLE type.
     */
    void setPreservingUnit(double d) {assertType(DOUBLE); dbl=d;}

    /**
     * Sets the measurement unit to the given value, leaving the numeric part
     * of the quantity unchanged. The object must already have the DOUBLE type.
     * The unit string pointer is expected to stay valid during the entire
     * duration of the simulation (see related class comment).
     */
    void setUnit(const char *unit);

    /**
     * Permanently converts this value to the given unit. The value must
     * already have the type DOUBLE. If the current unit cannot be converted
     * to the given one, an error will be thrown. The unit string pointer
     * is expected to stay valid during the entire simulation (see related
     * class comment).
     *
     * @see doubleValueInUnit()
     */
    void convertTo(const char *unit);

    /**
     * If this value is of type INT, converts it into DOUBLE; has no effect if
     * it is already a DOUBLE; and throws an error otherwise.
     */
    void convertToDouble();

    /**
     * Sets the value to the given string value. The string itself will be
     * copied. nullptr is also accepted and treated as an empty string.
     */
    void set(const char *s) {type=STRING; this->s=s?s:"";}

    /**
     * Sets the value to the given string value.
     */
    void set(const std::string& s) {type=STRING; this->s=s;}

    /**
     * Sets the value to the given string value.
     */
    void set(const opp_string& s) {type=STRING; this->s=s.c_str();}

    /**
     * Sets the value to the given pointer. The pointer is treated by
     * as an opaque value: If it points to an object, the object's ownership
     * is unaffected, and cValue will never delete or clone the object.
     */
    void set(any_ptr ptr) {type=POINTER; this->ptr=ptr;}

    /**
     * Sets the value to the given object, via set(any_ptr). Note that
     * cValue solely stores the object's pointer, and does nothing extra.
     * The object's ownership is unaffected, and cValue will never
     * delete or clone the object.
     */
    void set(cObject *obj) {set(toAnyPtr(obj));}

    /**
     * Deleted function to prevent non-cObject pointers from silently being converted to bool.
     */
    void set(const void *) = delete;
    //@}

    /** @name Getter functions. Note that overloaded conversion operators also exist. */
    //@{

    /**
     * Returns the value as a boolean. The type must be BOOL.
     */
    bool boolValue() const {assertType(BOOL); return bl;}

    /**
     * Returns a dimensionless value as an integer. The type must be INT.
     * This method cannot be used for values that have a unit (it will throw
     * an exception); for those, use either intValueInUnit() or intValueRaw()+getUnit().
     */
    intval_t intValue() const;

    /**
     * Returns value as an integer. The type must be INT. This method should be
     * used together with getUnit() to be able to make sense of the returned value.
     */
    intval_t intValueRaw() const;

    /**
     * Returns the numeric value as an integer converted to the given unit.
     * If the current unit cannot be converted to the given one, an error
     * will be thrown. The type must be INT.
     */
    intval_t intValueInUnit(const char *targetUnit) const;

    /**
     * Returns a dimensionless value as a double. The type must be DOUBLE or INT.
     * This method cannot be used for values that have a unit (it will throw
     * an exception); for those, use either doubleValueInUnit() or
     * doubleValueRaw()+getUnit().
     */
    double doubleValue() const;

    /**
     * Returns the value as a double. The type must be DOUBLE or INT. This method
     * should be used together with getUnit() to be able to make sense of the
     * returned value.
     */
    double doubleValueRaw() const;

    /**
     * Returns the numeric value as a double converted to the given unit.
     * If the current unit cannot be converted to the given one, an error
     * will be thrown. The type must be DOUBLE or INT.
     */
    double doubleValueInUnit(const char *targetUnit) const;

    /**
     * Returns the unit ("s", "mW", "Hz", "bps", etc), or nullptr if there was no
     * unit was specified. Unit is only valid for the DOUBLE and INT types.
     */
    const char *getUnit() const {return (type==DOUBLE || type==INT) ? unit.c_str() : nullptr;}

    /**
     * Returns value as const char *. The type must be STRING.
     */
    const char *stringValue() const {assertType(STRING); return s.c_str();}

    /**
     * Returns value as std::string. The type must be STRING.
     */
    const std::string& stdstringValue() const {assertType(STRING); return s;}

    /**
     * Returns value as any_ptr. The type must be POINTER.
     */
    any_ptr pointerValue() const {assertType(POINTER); return ptr;}

    /**
     * Returns true if the value is nullptr.
     */
    bool isNullptr() const {return type==cValue::POINTER && ptr == nullptr;}

    /**
     * Returns true if the value contains a (non-null) pointer to a cObject.
     */
    bool containsObject() const;

    /**
     * Returns true if the value contains a (non-null) pointer to a cXMLElement.
     */
    bool containsXML() const;

    /**
     * Returns value as pointer to cObject. The type must be POINTER, and
     * the pointer must point to an instance of cObject or be nullptr.
     */
    cObject *objectValue() const;

    /**
     * Returns value as pointer to cXMLElement. The type must be POINTER, and
     * the pointer must point to an instance of cXMLElement or be nullptr.
     */
    cXMLElement *xmlValue() const;
    //@}

    /** @name Overloaded assignment and conversion operators. */
    //@{

    /**
     * Equivalent to set(bool).
     */
    cValue& operator=(bool b)  {set(b); return *this;}

    /**
     * Converts the argument to long, and calls set(intval_t).
     */
    cValue& operator=(char c)  {set((intval_t)c); return *this;}

    /**
     * Converts the argument to long, and calls set(intval_t).
     */
    cValue& operator=(unsigned char c)  {set((intval_t)c); return *this;}

    /**
     * Converts the argument to long, and calls set(intval_t).
     */
    cValue& operator=(short i)  {set((intval_t)i); return *this;}

    /**
     * Converts the argument to long, and calls set(intval_t).
     */
    cValue& operator=(unsigned short i)  {set((intval_t)i); return *this;}

    /**
     * Converts the argument to long, and calls set(intval_t).
     */
    cValue& operator=(int i)  {set((intval_t)i); return *this;}

    /**
     * Converts the argument to long, and calls set(intval_t).
     */
    cValue& operator=(unsigned int i)  {set((intval_t)i); return *this;}

    /**
     * Equivalent to set(intval_t).
     */
    cValue& operator=(long l)  {set((intval_t)l); return *this;}

    /**
     * Converts the argument to long, and calls set(intval_t).
     */
    cValue& operator=(unsigned long l) {set(checked_int_cast<intval_t>(l, OVERFLOW_MSG)); return *this;}

    /**
     * Equivalent to set(intval_t).
     */
    cValue& operator=(long long l)  {set(checked_int_cast<intval_t>(l, OVERFLOW_MSG)); return *this;}

    /**
     * Converts the argument to long, and calls set(intval_t).
     */
    cValue& operator=(unsigned long long l) {set(checked_int_cast<intval_t>(l, OVERFLOW_MSG)); return *this;}

    /**
     * Equivalent to set(double).
     */
    cValue& operator=(double d)  {set(d); return *this;}

    /**
     * Converts the argument to double, and calls set(double).
     */
    cValue& operator=(long double d)  {set((double)d); return *this;}

    /**
     * Equivalent to set(const char *).
     */
    cValue& operator=(const char *s)  {set(s); return *this;}

    /**
     * Equivalent to set(const std::string&).
     */
    cValue& operator=(const std::string& s)  {set(s); return *this;}

    /**
     * Equivalent to set(any_ptr).
     */
    cValue& operator=(any_ptr ptr)  {set(ptr); return *this;}

    /**
     * Equivalent to set(cObject *).
     */
    cValue& operator=(cObject *obj)  {set(obj); return *this;}

    /**
     * Equivalent to boolValue().
     */
    operator bool() const  {return boolValue();}

    /**
     * Calls intValue() and converts the result to char.
     * An exception is thrown if the conversion would result in a data loss,
     */
    operator char() const  {return checked_int_cast<char>(intValue(), OVERFLOW_MSG);}

    /**
     * Calls intValue() and converts the result to unsigned char.
     * An exception is thrown if the conversion would result in a data loss,
     */
    operator unsigned char() const  {return checked_int_cast<unsigned char>(intValue(), OVERFLOW_MSG);}

    /**
     * Calls intValue() and converts the result to int.
     * An exception is thrown if the conversion would result in a data loss,
     */
    operator int() const  {return checked_int_cast<int>(intValue(), OVERFLOW_MSG);}

    /**
     * Calls intValue() and converts the result to unsigned int.
     * An exception is thrown if the conversion would result in a data loss,
     */
    operator unsigned int() const  {return checked_int_cast<unsigned int>(intValue(), OVERFLOW_MSG);}

    /**
     * Calls intValue() and converts the result to short.
     * An exception is thrown if the conversion would result in a data loss,
     */
    operator short() const  {return checked_int_cast<short>(intValue(), OVERFLOW_MSG);}

    /**
     * Calls intValue() and converts the result to unsigned short.
     * An exception is thrown if the conversion would result in a data loss,
     */
    operator unsigned short() const  {return checked_int_cast<unsigned short>(intValue(), OVERFLOW_MSG);}

    /**
     * Calls intValue() and converts the result to long.
     * An exception is thrown if the conversion would result in a data loss,
     */
    operator long() const  {return checked_int_cast<long>(intValue(), OVERFLOW_MSG);}

    /**
     * Calls intValue() and converts the result to unsigned long.
     * An exception is thrown if the conversion would result in a data loss,
     */
    operator unsigned long() const  {return checked_int_cast<unsigned long>(intValue(), OVERFLOW_MSG);}

    /**
     * Calls intValue() and converts the result to long long.
     * An exception is thrown if the conversion would result in a data loss,
     */
    operator long long() const  {return checked_int_cast<long long>(intValue(), OVERFLOW_MSG);}

    /**
     * Calls intValue() and converts the result to unsigned long long.
     * An exception is thrown if the conversion would result in a data loss,
     */
    operator unsigned long long() const  {return checked_int_cast<unsigned long long>(intValue(), OVERFLOW_MSG);}

    /**
     * Equivalent to doubleValue().
     */
    operator double() const  {return doubleValue();}

    /**
     * Calls doubleValue() and converts the result to long double.
     * Note that this is a potentially lossy operation.
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
     * Equivalent to pointerValue().
     */
    operator any_ptr() const  {return pointerValue();}

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

// cNedValue was renamed cValue in OMNeT++ 6.0; typedef added for compatibility
typedef cValue cNedValue;
typedef cValue cNEDValue;

}  // namespace omnetpp

#endif


