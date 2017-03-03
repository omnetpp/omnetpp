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

namespace omnetpp {

class cPar;
class cXMLElement;
class cDynamicExpression;

/**
 * @brief Value used during evaluating NED expressions.
 *
 * See notes below.
 *
 * <b>Double vs long:</b>
 *
 * There is no <tt>long</tt> field in cNEDValue; the reason is that the NED
 * expression evaluator stores all numbers as <tt>double</tt>. The IEEE double's
 * mantissa is 53 bits, so <tt>double</tt> can accurately represent <tt>long</tt>
 * on 32-bit architectures. On 64-bit architectures the usual size of
 * <tt>long</tt> is 64 bits, so precision loss will occur when converting
 * very large integers to <tt>double</tt>. Note, however, that simulations
 * that trigger this precision loss would not be able to run on 32-bit
 * architectures at all.
 *
 * <b>Measurement unit strings:</b>
 *
 * For performance reasons, the functions that store a measurement unit
 * will only store the <tt>const char *</tt> pointer and not copy the
 * string itself. Consequently, the passed unit pointers must stay valid
 * at least during the lifetime of the cNEDValue object, or even longer
 * if the same pointer propagates to other cNEDValue objects. It is recommended
 * that you only pass pointers that stay valid during the entire simulation.
 * It is safe to use: (1) string constants from the code; (2) units strings
 * from other cNEDValues; and (3) stringpooled strings, e.g. from the
 * getPooled() method or from cStringPool.
 *
 * @see cDynamicExpression, cNEDFunction, Define_NED_Function()
 * @ingroup SimSupport
 */
class SIM_API cNEDValue
{
    friend class cDynamicExpression;
  public:
    /**
     * Type of the value stored in a cNEDValue object.
     */
    // Note: char codes need to be present and be consistent with cNEDFunction::getArgTypes()!
    enum Type {UNDEF=0, BOOL='B', DBL='D', STR='S', XML='X'} type;

  private:
    bool bl;
    double dbl;
    const char *dblunit; // string constants or pooled strings; may be nullptr
    std::string s;
    cXMLElement *xml;

  private:
#ifdef NDEBUG
    void assertType(Type) const {}
#else
    void assertType(Type t) const {if (type!=t) cannotCastError(t);}
#endif
    void cannotCastError(Type t) const;

  public:
    /** @name Constructors */
    //@{
    cNEDValue()  {type=UNDEF;}
    cNEDValue(bool b)  {set(b);}
    cNEDValue(long l)  {set(l);}
    cNEDValue(double d)  {set(d);}
    cNEDValue(double d, const char *unit)  {set(d,unit);}
    cNEDValue(const char *s)  {set(s);}
    cNEDValue(const std::string& s)  {set(s);}
    cNEDValue(cXMLElement *x)  {set(x);}
    cNEDValue(const cPar& par) {set(par);}
    //@}

    /**
     * Assignment
     */
    void operator=(const cNEDValue& other);

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
    bool isNumeric() const {return type==DBL;}

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
     * unit strings suitable for cNEDValue; see related class comment.
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
     * Sets the value to the given long value.
     */
    void set(long l) {type=DBL; dbl=l; dblunit=nullptr;}

    /**
     * Sets the value to the given double value and measurement unit.
     * The unit string pointer is expected to stay valid during the entire
     * duration of the simulation (see related class comment).
     */
    void set(double d, const char *unit=nullptr) {type=DBL; dbl=d; dblunit=unit;}

    /**
     * Sets the value to the given double value, preserving the current
     * measurement unit. The object must already have the DBL type.
     */
    void setPreservingUnit(double d) {assertType(DBL); dbl=d;}

    /**
     * Sets the measurement unit to the given value, leaving the numeric part
     * of the quantity unchanged. The object must already have the DBL type.
     * The unit string pointer is expected to stay valid during the entire
     * duration of the simulation (see related class comment).
     */
    void setUnit(const char *unit) {assertType(DBL); dblunit=unit;}

    /**
     * Permanently converts this value to the given unit. The value must
     * already have the type DBL. If the current unit cannot be converted
     * to the given one, an error will be thrown. The unit string pointer
     * is expected to stay valid during the entire simulation (see related
     * class comment).
     *
     * @see doubleValueInUnit()
     */
    void convertTo(const char *unit);

    /**
     * Sets the value to the given string value. The string itself will be
     * copied. nullptr is also accepted and treated as an empty string.
     */
    void set(const char *s) {type=STR; this->s=s?s:"";}

    /**
     * Sets the value to the given string value.
     */
    void set(const std::string& s) {type=STR; this->s=s;}

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
     * Returns value as long. The type must be DBL (Note: there is no LONG.)
     */
    long longValue() const {assertType(DBL); return (long)dbl;}

    /**
     * Returns value as double. The type must be DBL.
     */
    double doubleValue() const {assertType(DBL); return dbl;}

    /**
     * Returns the numeric value as a double converted to the given unit.
     * If the current unit cannot be converted to the given one, an error
     * will be thrown. The type must be DBL.
     */
    double doubleValueInUnit(const char *unit) const {return convertUnit(dbl, dblunit, unit);}

    /**
     * Returns the unit ("s", "mW", "Hz", "bps", etc), or nullptr if there was no
     * unit was specified. Unit is only valid for the DBL type.
     */
    const char *getUnit() const {assertType(DBL); return dblunit;}

    /**
     * Returns value as const char *. The type must be STR.
     */
    const char *stringValue() const {assertType(STR); return s.c_str();}

    /**
     * Returns value as std::string. The type must be STR.
     */
    const std::string& stdstringValue() const {assertType(STR); return s;}

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
    cNEDValue& operator=(bool b)  {set(b); return *this;}

    /**
     * Converts the argument to long, and calls set(long).
     */
    cNEDValue& operator=(char c)  {set((long)c); return *this;}

    /**
     * Converts the argument to long, and calls set(long).
     */
    cNEDValue& operator=(unsigned char c)  {set((long)c); return *this;}

    /**
     * Converts the argument to long, and calls set(long).
     */
    cNEDValue& operator=(int i)  {set((long)i); return *this;}

    /**
     * Converts the argument to long, and calls set(long).
     */
    cNEDValue& operator=(unsigned int i)  {set((long)i); return *this;}

    /**
     * Converts the argument to long, and calls set(long).
     */
    cNEDValue& operator=(short i)  {set((long)i); return *this;}

    /**
     * Converts the argument to long, and calls set(long).
     */
    cNEDValue& operator=(unsigned short i)  {set((long)i); return *this;}

    /**
     * Equivalent to set(long).
     */
    cNEDValue& operator=(long l)  {set(l); return *this;}

    /**
     * Converts the argument to long, and calls set(long).
     */
    cNEDValue& operator=(unsigned long l) {set((long)l); return *this;}

    /**
     * Equivalent to setDoubleValue().
     */
    cNEDValue& operator=(double d)  {set(d); return *this;}

    /**
     * Converts the argument to double, and calls set(double).
     */
    cNEDValue& operator=(long double d)  {set((double)d); return *this;}

    /**
     * Equivalent to set(const char *).
     */
    cNEDValue& operator=(const char *s)  {set(s); return *this;}

    /**
     * Equivalent to set(const std::string&).
     */
    cNEDValue& operator=(const std::string& s)  {set(s); return *this;}

    /**
     * Equivalent to set(cXMLElement *).
     */
    cNEDValue& operator=(cXMLElement *node)  {set(node); return *this;}

    /**
     * Equivalent to set(const cPar&).
     */
    cNEDValue& operator=(const cPar& par)  {set(par); return *this;}

    /**
     * Equivalent to boolValue().
     */
    operator bool() const  {return boolValue();}

    /**
     * Calls longValue() and converts the result to char.
     * Note that this is a potentially lossy operation.
     */
    operator char() const  {return (char)longValue();}

    /**
     * Calls longValue() and converts the result to unsigned char.
     * Note that this is a potentially lossy operation.
     */
    operator unsigned char() const  {return (unsigned char)longValue();}

    /**
     * Calls longValue() and converts the result to int.
     * Note that this is a potentially lossy operation.
     */
    operator int() const  {return (int)longValue();}

    /**
     * Calls longValue() and converts the result to unsigned int.
     * Note that this is a potentially lossy operation.
     */
    operator unsigned int() const  {return (unsigned int)longValue();}

    /**
     * Calls longValue() and converts the result to short.
     * Note that this is a potentially lossy operation.
     */
    operator short() const  {return (short)longValue();}

    /**
     * Calls longValue() and converts the result to unsigned short.
     * Note that this is a potentially lossy operation.
     */
    operator unsigned short() const  {return (unsigned short)longValue();}

    /**
     * Equivalent to longValue().
     */
    operator long() const  {return longValue();}

    /**
     * Calls longValue() and converts the result to unsigned long.
     * Note that the result may be misleading!
     */
    operator unsigned long() const  {return longValue();}

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

}  // namespace omnetpp

#endif


