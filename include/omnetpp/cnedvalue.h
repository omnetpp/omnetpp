//==========================================================================
//   CNEDVALUE.H  - part of
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

#ifndef __OMNETPP_CNEDVALUE_H
#define __OMNETPP_CNEDVALUE_H

#include <string>
#include "simkerneldefs.h"
#include "cexception.h"
#include "simutil.h"

namespace omnetpp {

class cPar;
class cXMLElement;
class cDynamicExpression;

/**
 * @brief Value used during evaluating NED expressions.
 *
 * See notes below.
 *
 * <b>Measurement unit strings:</b>
 *
 * For performance reasons, the functions that store a measurement unit
 * will only store the <tt>const char *</tt> pointer and not copy the
 * string itself. Consequently, the passed unit pointers must stay valid
 * at least during the lifetime of the cNedValue object, or even longer
 * if the same pointer propagates to other cNedValue objects. It is recommended
 * that you only pass pointers that stay valid during the entire simulation.
 * It is safe to use: (1) string constants from the code; (2) units strings
 * from other cNedValues; and (3) stringpooled strings, e.g. from the
 * getPooled() method or from cStringPool.
 *
 * @see cDynamicExpression, cNedFunction, Define_NED_Function()
 * @ingroup SimSupport
 */
class SIM_API cNedValue
{
    friend class cDynamicExpression;
  public:
    /**
     * Type of the value stored in a cNedValue object.
     */
    // Note: char codes need to be present and be consistent with cNedFunction::getArgTypes()!
    enum Type {UNDEF=0, BOOL='B', INT='L', DOUBLE='D', STRING='S', XML='X', DBL=DOUBLE, STR=STRING} type;

  private:
    bool bl;
    intpar_t intv;
    double dbl;
    const char *unit; // for INT/DOUBLE; must point to string constant or pooled string; may be nullptr
    std::string s;
    cXMLElement *xml;
    static const char *OVERFLOW_MSG;

  private:
#ifdef NDEBUG
    void assertType(Type) const {}
#else
    void assertType(Type t) const {if (type!=t) cannotCastError(t);}
#endif
    [[noreturn]] void cannotCastError(Type t) const;

  public:
    /** @name Constructors */
    //@{
    cNedValue()  {type=UNDEF;}
    cNedValue(bool b)  {set(b);}
    cNedValue(int l)  {set((intpar_t)l);}
    cNedValue(int l, const char *unit)  {set((intpar_t)l, unit);}
    cNedValue(intpar_t l)  {set(l);}
    cNedValue(intpar_t l, const char *unit)  {set(l, unit);}
    cNedValue(double d)  {set(d);}
    cNedValue(double d, const char *unit)  {set(d,unit);}
    cNedValue(const char *s)  {set(s);}
    cNedValue(const std::string& s)  {set(s);}
    cNedValue(cXMLElement *x)  {set(x);}
    cNedValue(const cPar& par) {set(par);}
    //@}

    /**
     * Assignment
     */
    void operator=(const cNedValue& other);

    /** @name Type, unit conversion and misc. */
    //@{
    /**
     * Returns the value type.
     */
    Type getType() const  {return type;}

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
     * Syntax: <number> | (<number> <unit>)+
     *
     * If there is a syntax error, or if unit mismatch is found (i.e. distance
     * is given instead of time), the method throws an exception.
     */
    static double parseQuantity(const char *str, std::string& outActualUnit);

    /**
     * Returns a copy of the string that is guaranteed to stay valid
     * until the program exits. Multiple calls with identical strings as
     * parameter will return the same copy. Useful for getting measurement
     * unit strings suitable for cNedValue; see related class comment.
     *
     * @see cStringPool, setUnit(), convertTo()
     */
    static const char *getPooled(const char *s);
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
    void set(intpar_t l, const char *unit=nullptr) {type=INT; intv=l; this->unit=unit;}

    /**
     * Sets the value to the given integer value and measurement unit (optional).
     * The unit string pointer is expected to stay valid during the entire
     * duration of the simulation (see related class comment).
     * This is a convenience method that delegates to the intpar_t version.
     */
    void set(int l, const char *unit=nullptr) {set((intpar_t)l, unit);}

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
    void setPreservingUnit(intpar_t l) {assertType(INT); intv=l;}

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
    void setUnit(const char* unit);

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
     * Sets the value to the given cXMLElement.
     */
    void set(cXMLElement *x) {type=XML; xml=x;}

    /**
     * Copies the value from a cPar.
     */
    void set(const cPar& par);
    //@}

    /** @name Getter functions. Note that overloaded conversion operators also exist. */
    //@{

    /**
     * Returns value as a boolean. The type must be BOOL.
     */
    bool boolValue() const {assertType(BOOL); return bl;}

    /**
     * Returns value as integer. The type must be INT or DOUBLE.
     */
    intpar_t intValue() const;

    /**
     * For compatibility; delegates to intValue().
     */
    _OPPDEPRECATED intpar_t longValue() const {return intValue();}

    /**
     * Returns the numeric value as an integer converted to the given unit.
     * If the current unit cannot be converted to the given one, an error
     * will be thrown. The type must be DOUBLE or INT.
     */
    intpar_t intValueInUnit(const char *targetUnit) const;

    /**
     * Returns value as double. The type must be DOUBLE or INT.
     */
    double doubleValue() const;

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
    const char *getUnit() const {return (type==DOUBLE || type==INT) ? unit : nullptr;}

    /**
     * Returns value as const char *. The type must be STRING.
     */
    const char *stringValue() const {assertType(STRING); return s.c_str();}

    /**
     * Returns value as std::string. The type must be STRING.
     */
    const std::string& stdstringValue() const {assertType(STRING); return s;}

    /**
     * Returns value as pointer to cXMLElement. The type must be XML.
     */
    cXMLElement *xmlValue() const {assertType(XML); return xml;}
    //@}

    /** @name Overloaded assignment and conversion operators. */
    //@{

    /**
     * Equivalent to set(bool).
     */
    cNedValue& operator=(bool b)  {set(b); return *this;}

    /**
     * Converts the argument to long, and calls set(intpar_t).
     */
    cNedValue& operator=(char c)  {set((intpar_t)c); return *this;}

    /**
     * Converts the argument to long, and calls set(intpar_t).
     */
    cNedValue& operator=(unsigned char c)  {set((intpar_t)c); return *this;}

    /**
     * Converts the argument to long, and calls set(intpar_t).
     */
    cNedValue& operator=(short i)  {set((intpar_t)i); return *this;}

    /**
     * Converts the argument to long, and calls set(intpar_t).
     */
    cNedValue& operator=(unsigned short i)  {set((intpar_t)i); return *this;}

    /**
     * Converts the argument to long, and calls set(intpar_t).
     */
    cNedValue& operator=(int i)  {set((intpar_t)i); return *this;}

    /**
     * Converts the argument to long, and calls set(intpar_t).
     */
    cNedValue& operator=(unsigned int i)  {set((intpar_t)i); return *this;}

    /**
     * Equivalent to set(intpar_t).
     */
    cNedValue& operator=(long l)  {set((intpar_t)l); return *this;}

    /**
     * Converts the argument to long, and calls set(intpar_t).
     */
    cNedValue& operator=(unsigned long l) {set(checked_int_cast<intpar_t>(l, OVERFLOW_MSG)); return *this;}

    /**
     * Equivalent to set(intpar_t).
     */
    cNedValue& operator=(long long l)  {set(checked_int_cast<intpar_t>(l, OVERFLOW_MSG)); return *this;}

    /**
     * Converts the argument to long, and calls set(intpar_t).
     */
    cNedValue& operator=(unsigned long long l) {set(checked_int_cast<intpar_t>(l, OVERFLOW_MSG)); return *this;}

    /**
     * Equivalent to set(double).
     */
    cNedValue& operator=(double d)  {set(d); return *this;}

    /**
     * Converts the argument to double, and calls set(double).
     */
    cNedValue& operator=(long double d)  {set((double)d); return *this;}

    /**
     * Equivalent to set(const char *).
     */
    cNedValue& operator=(const char *s)  {set(s); return *this;}

    /**
     * Equivalent to set(const std::string&).
     */
    cNedValue& operator=(const std::string& s)  {set(s); return *this;}

    /**
     * Equivalent to set(cXMLElement *).
     */
    cNedValue& operator=(cXMLElement *node)  {set(node); return *this;}

    /**
     * Equivalent to set(const cPar&).
     */
    cNedValue& operator=(const cPar& par)  {set(par); return *this;}

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
     * Equivalent to xmlValue(). NOTE: The lifetime of the returned object tree
     * is limited; see xmlValue() for details.
     */
    operator cXMLElement *() const  {return xmlValue();}
    //@}
};

// cNEDValue was renamed cNedValue in OMNeT++ 5.3; typedef added for compatibility
typedef cNedValue cNEDValue;

}  // namespace omnetpp

#endif


