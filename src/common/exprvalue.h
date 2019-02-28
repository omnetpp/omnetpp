//==========================================================================
//  EXPRVALUE.H  - part of
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

#ifndef __OMNETPP_COMMON_EXPRVALUE_H
#define __OMNETPP_COMMON_EXPRVALUE_H

#include <string>
#include "commondefs.h"
#include "intutil.h"
#include "stringutil.h"

namespace omnetpp { class cObject; }

namespace omnetpp {
namespace common {

class MatchExpression;

namespace expression {

/**
 * @brief Value used during evaluating expressions.
 *
 * See notes below.
 *
 * <b>Measurement unit strings:</b>
 *
 * For performance reasons, the functions that store a measurement unit
 * will only store the <tt>const char *</tt> pointer and not copy the
 * string itself. Consequently, the passed unit pointers must stay valid
 * at least during the lifetime of the ExprValue object, or even longer
 * if the same pointer propagates to other ExprValue objects. It is recommended
 * that you only pass pointers that stay valid during the entire simulation.
 * It is safe to use: (1) string constants from the code; (2) units strings
 * from other ExprValue's; and (3) stringpooled strings, e.g. from the
 * getPooled() method or from StringPool.
 */
class COMMON_API ExprValue
{
    friend class ExprNode;
    friend class NegateNode;
    friend class NotNode;
    friend class BitwiseNotNode;
    friend class AddNode;
    friend class SubNode;
    friend class MulNode;
    friend class DivNode;
    friend class ModNode;
    friend class PowNode;
    friend class CompareNode;
    friend class MatchNode;
    friend class MatchConstPatternNode;
    friend class InlineIfNode;
    friend class LogicalInfixOperatorNode;
    friend class BitwiseInfixOperatorNode;
    friend class MathFunc1Node;
    friend class MathFunc2Node;
    friend class MathFunc3Node;
    friend class MathFunc4Node;
    friend class FunctionNode;
    friend class omnetpp::common::MatchExpression;

  public:
    enum Type {UNDEF=0, BOOL='B', INT='L', DOUBLE='D', STRING='S', OBJECT='O'};

  private:
    Type type = UNDEF;
    union {
        bool bl;
        intpar_t intv;
        double dbl;
        cObject *obj;
        const char *s; // non-nullptr, dynamically allocated
    };
    const char *unit=nullptr; // for INT/DOUBLE; must point to string constant or pooled string; may be nullptr
    static std::string (*objectToString)(cObject *); // method to print info about cObjects

  private:
#ifdef NDEBUG
    void assertType(Type) const {}
#else
    void assertType(Type t) const {if (type!=t) cannotCastError(t);}
#endif
    char *strdup(const char *s) {char *p = new char[strlen(s)+1]; strcpy(p,s); return p;}
    void deleteOld() {if (type==STRING) delete[] s;}
    [[noreturn]] void cannotCastError(Type t) const;

  public:
    /** @name Constructors */
    //@{
    ExprValue() {}
    ExprValue(const ExprValue& other) {operator=(other);}
    ExprValue(ExprValue&& other) {operator=(other);}
    ExprValue(bool b)  {operator=(b);}
    ExprValue(intpar_t l)  {operator=(l);}
    ExprValue(intpar_t l, const char *unit)  {setQuantity(l, unit);}
    ExprValue(double d)  {operator=(d);}
    ExprValue(double d, const char *unit)  {setQuantity(d,unit);}
    ExprValue(const char *s)  {operator=(s);}
    ExprValue(const std::string& s)  {operator=(s);}
    ExprValue(cObject *x)  {operator=(x);}
    ~ExprValue()  {if (type==STRING) delete[] s;}
    //@}

    /**
     * Assignment
     */
    void operator=(const ExprValue& other);
    void operator=(ExprValue&& other);

    /** @name Misc access. */
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
     * Returns the value in text form.
     */
    std::string str() const;

    /**
     * Returns a copy of the string that is guaranteed to stay valid
     * until the program exits. Multiple calls with identical strings as
     * parameter will return the same copy. Useful for getting measurement
     * unit strings suitable for ExprValue; see related class comment.
     *
     * @see StringPool, setUnit(), convertTo()
     */
    static const char *getPooled(const char *s);

    /**
     * Sets method to print info about cObjects. This needs to be called
     * once on startup from the simulation library which defines cObject.
     */
    static void setObjectStrFunction(std::string (*f)(cObject *)) {objectToString = f;}
    //@}

    /** @name Setting the value. */
    //@{

    /**
     * Sets the value to the given bool value.
     */
    /**
     * Sets the value to the given bool value.
     */
    ExprValue& operator=(bool b)  {deleteOld(); type=BOOL; bl=b; return *this;}

    /**
     * Sets the value to the given integer value.
     */
    ExprValue& operator=(intpar_t d)  {deleteOld(); type=INT; intv=d; unit=nullptr; return *this;}

    /**
     * Sets the value to the given double value.
     */
    ExprValue& operator=(double d)  {deleteOld(); type=DOUBLE; dbl=d; unit=nullptr;; return *this;}

    /**
     * Sets the value to the given string value. nullptr is not accepted.
     */
    ExprValue& operator=(const char *s)  {Assert(s); deleteOld(); type=STRING; this->s=strdup(s); return *this;}

    /**
     * Sets the value to the given string value.
     */
    ExprValue& operator=(const std::string& s)  {deleteOld(); type=STRING; this->s=strdup(s.c_str()); return *this;}

    /**
     * Sets the value to the given cObject.
     */
    ExprValue& operator=(cObject *x)  {deleteOld(); type=OBJECT; obj=x; return *this;}

    /**
     * Sets the value to the given integer value and measurement unit.
     * The unit string pointer is expected to stay valid during the entire
     * duration of the simulation (see related class comment).
     */
    void setQuantity(intpar_t d, const char *unit) {deleteOld(); type=INT; intv=d; this->unit=unit;}

    /**
     * Sets the value to the given double value and measurement unit.
     * The unit string pointer is expected to stay valid during the entire
     * duration of the simulation (see related class comment).
     */
    void setQuantity(double d, const char *unit) {deleteOld(); type=DOUBLE; dbl=d; this->unit=unit;}

    /**
     * Sets the value to the given integer value, preserving the current
     * measurement unit. The object must already have the INT type.
     */
    void setPreservingUnit(intpar_t d) {assertType(INT); intv=d;}

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
    //@}

    /** @name Accessing the value. */
    //@{

    /**
     * Returns value as a boolean. The type must be BOOL.
     */
    bool boolValue() const {assertType(BOOL); return bl;}

    /**
     * Returns value as integer. The type must be INT.
     */
    intpar_t intValue() const;

    /**
     * Returns the numeric value as an integer converted to the given unit.
     * If the current unit cannot be converted to the given one, an error
     * will be thrown. The type must be INT.
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
    const char *stringValue() const {assertType(STRING); return s;}

    /**
     * Returns value as pointer to cObject. The type must be OBJECT.
     */
    cObject *objectValue() const {assertType(OBJECT); return obj;}

    /**
     * Equivalent to boolValue().
     */
    operator bool() const  {return boolValue();}

    /**
     * Equivalent to intValue().
     * Calls intValue() and converts the result to unsigned long long.
     * An exception is thrown if the conversion would result in a data loss,
     */
    operator intpar_t() const  {return intValue();}

    /**
     * Equivalent to doubleValue().
     */
    operator double() const  {return doubleValue();}

    /**
     * Equivalent to stringValue().
     */
    operator const char *() const  {return stringValue();}

    /**
     * Equivalent to objectValue().
     */
    operator cObject *() const  {return objectValue();}
    //@}
};

}  // namespace expression
}  // namespace common
}  // namespace omnetpp

#endif


