//==========================================================================
//   CNEDVALUE.CC  - part of
//                     OMNeT++/OMNEST
//            Discrete System Simulation in C++
//
//  Author: Andras Varga
//
//==========================================================================

/*--------------------------------------------------------------*
  Copyright (C) 1992-2017 Andras Varga
  Copyright (C) 2006-2017 OpenSim Ltd.

  This file is distributed WITHOUT ANY WARRANTY. See the file
  `license' for details on this and other legal matters.
*--------------------------------------------------------------*/

#include <cinttypes>  // PRId64
#include "common/stringutil.h"
#include "common/stringpool.h"
#include "common/unitconversion.h"
#include "omnetpp/cnedvalue.h"
#include "omnetpp/cxmlelement.h"
#include "omnetpp/cexception.h"
#include "omnetpp/cpar.h"

using namespace omnetpp::common;

namespace omnetpp {

const char *cNedValue::OVERFLOW_MSG = "Integer overflow casting %s to a smaller or unsigned integer type";

void cNedValue::operator=(const cNedValue& other)
{
    type = other.type;
    switch (type) {
        case UNDEF: break;
        case BOOL: bl = other.bl; break;
        case INT: intv = other.intv; unit = other.unit; break;
        case DOUBLE: dbl = other.dbl; unit = other.unit; break;
        case STRING: s = other.s; break;
        case XML: xml = other.xml; break;
    }
}

const char *cNedValue::getTypeName(Type t)
{
    switch (t) {
        case UNDEF:  return "undef";
        case BOOL:   return "bool";
        case INT:    return "integer";
        case DOUBLE: return "double";
        case STRING: return "string";
        case XML:    return "xml";
        default:     return "???";
    }
}

void cNedValue::cannotCastError(Type targetType) const
{
    const char *note = (getType() == DOUBLE && targetType==INT) ?
            " (note: no implicit conversion from double to int)" : "";
    throw cRuntimeError("Cannot cast %s from type %s to %s%s",
            str().c_str(), getTypeName(type), getTypeName(targetType), note);
}

void cNedValue::set(const cPar& par)
{
    switch (par.getType()) {
        case cPar::BOOL: *this = par.boolValue(); break;
        case cPar::INT: *this = par.intValue(); unit = par.getUnit(); break;
        case cPar::DOUBLE: *this = par.doubleValue(); unit = par.getUnit(); break;
        case cPar::STRING: *this = par.stdstringValue(); break;
        case cPar::XML: *this = par.xmlValue(); break;
        default: throw cRuntimeError("Internal error: Invalid cPar type: %s", par.getFullPath().c_str());
    }
}

intpar_t cNedValue::intValue() const
{
    if (type == INT)
        return intv;
    else
        cannotCastError(INT);
}

inline double safeCastToDouble(intpar_t x)
{
    double d = (double)x;
    intpar_t x2 = (intpar_t)d;
    if (x != x2)
        throw cRuntimeError("Integer %" PRId64 " too large, conversion to double would incur precision loss (hint: if this occurs in NED or ini, use the double() operator to suppress this error)", (int64_t)x);
    return d;
}

void cNedValue::convertToDouble()
{
    if (type == INT) {
        type = DOUBLE;
        dbl = safeCastToDouble(intv);
    }
    else if (type != DOUBLE)
        cannotCastError(DOUBLE);
}

//TODO these should be in some utils.cc file

#ifndef __has_builtin
  #define __has_builtin(x) 0  // Compatibility for compilers without the __has_builtin macro
#endif

inline intpar_t safeMul(intpar_t a, intpar_t b)
{
#if (__has_builtin(__builtin_mul_overflow) && !defined(__c2__)) || (defined(__GNUC__) && !defined(__clang__) && __GNUC__ >= 5)
    intpar_t res;
    if ( __builtin_mul_overflow(a, b, &res))
        throw cRuntimeError("Integer overflow multiplying %" PRId64 " and %" PRId64 ", try converting to doubles", (int64_t)a, (int64_t)b);
    return res;
#else
    return a * b;  // unchecked
#endif
}

intpar_t cNedValue::intValueInUnit(const char *targetUnit) const
{
    if (type == INT) {
        if (intv == 0 && opp_isempty(unit))
            return 0;  // accept 0 without unit
        double c = UnitConversion::getConversionFactor(getUnit(), targetUnit);
        if (c == 1)
            return intv;
        else if (c > 1 && c == floor(c))
            return safeMul((intpar_t)c, intv);
        else
            throw cRuntimeError("Cannot convert integer from unit %s to %s: no conversion or conversion rate is not integer", getUnit(), targetUnit);
    }
    else {
        cannotCastError(INT);
    }
}

double cNedValue::doubleValue() const
{
    if (type == DOUBLE)
        return dbl;
    else if (type == INT)
        return intv;
    else
        cannotCastError(DOUBLE);
    return 0;
}

double cNedValue::doubleValueInUnit(const char *targetUnit) const
{
    if (type == DOUBLE)
        return UnitConversion::convertUnit(dbl, unit, targetUnit);
    else if (type == INT)
        return UnitConversion::convertUnit(safeCastToDouble(intv), unit, targetUnit);
    else
        cannotCastError(DOUBLE);
    return 0;
}

void cNedValue::convertTo(const char *targetUnit)
{
    assertType(DOUBLE);
    dbl = UnitConversion::convertUnit(dbl, unit, targetUnit);
    unit = targetUnit;
}

void cNedValue::setUnit(const char* unit)
{
    if (type != DOUBLE && type != INT)
        throw cRuntimeError("Cannot set measurement unit on a value of type %s", getTypeName(type));
    this->unit = unit;
}


double cNedValue::convertUnit(double d, const char *unit, const char *targetUnit)
{
    // not inline because simkernel header files cannot refer to common/ headers (unitconversion.h)
    return UnitConversion::convertUnit(d, unit, targetUnit);
}

double cNedValue::parseQuantity(const char *str, const char *expectedUnit)
{
    return UnitConversion::parseQuantity(str, expectedUnit);
}

double cNedValue::parseQuantity(const char *str, std::string& outActualUnit)
{
    return UnitConversion::parseQuantity(str, outActualUnit);
}

const char *cNedValue::getPooled(const char *s)
{
    static StringPool stringPool;  // non-refcounted
    return stringPool.get(s);
}

std::string cNedValue::str() const
{
    char buf[32];
    switch (type) {
        case BOOL: return bl ? "true" : "false";
        case INT: sprintf(buf, "%" PRId64 "%s", (int64_t)intv, opp_nulltoempty(unit)); return buf;
        case DOUBLE:
            opp_dtoa(buf, "%g", dbl);
            if (!std::isfinite(dbl))
                strcat(buf, " ");
            if (!opp_isempty(unit))
                strcat(buf, unit);
            return buf;
        case STRING: return opp_quotestr(s);
        case XML: return xml->str();
        default: throw cRuntimeError("Internal error: Invalid cNedValue type");
    }
}

}  // namespace omnetpp

