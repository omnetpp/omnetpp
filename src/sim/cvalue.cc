//==========================================================================
//   CVALUE.CC  - part of
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

#include <cinttypes>  // PRId64
#include "common/stringutil.h"
#include "common/pooledstring.h"
#include "common/unitconversion.h"
#include "common/intutil.h"
#include "omnetpp/cvalue.h"
#include "omnetpp/cxmlelement.h"
#include "omnetpp/cexception.h"
#include "omnetpp/cpar.h"
#include "omnetpp/checkandcast.h"
#include "omnetpp/cvaluearray.h"
#include "omnetpp/cvaluemap.h"
#include "omnetpp/checkandcast.h"

using namespace omnetpp::common;

namespace omnetpp {

const char *cValue::OVERFLOW_MSG = "Integer overflow casting %s to a smaller or unsigned integer type";

void cValue::operator=(const cValue& other)
{
    type = other.type;
    switch (type) {
        case UNDEF: break;
        case BOOL: bl = other.bl; break;
        case INT: intv = other.intv; unit = other.unit; break;
        case DOUBLE: dbl = other.dbl; unit = other.unit; break;
        case STRING: s = other.s; break;
        case POINTER: ptr = other.ptr; break;
    }
}

const char *cValue::getTypeName(Type t)
{
    switch (t) {
        case UNDEF:  return "undefined";
        case BOOL:   return "bool";
        case INT:    return "integer";
        case DOUBLE: return "double";
        case STRING: return "string";
        case POINTER: return "object"; // kinda confusing, I know -- but it needs to be consistent with NED param type, NED function arg type etc.
        default:     return "???";
    }
}

void cValue::cannotCastError(Type targetType) const
{
    const char *note = (getType() == DOUBLE && targetType==INT) ?
            " (note: no implicit conversion from double to int)" : "";
    throw cRuntimeError("Cannot cast %s from type %s to %s%s",
            str().c_str(), getTypeName(type), getTypeName(targetType), note);
}

intval_t cValue::intValue() const
{
    if (type != INT)
        cannotCastError(INT);
    if (!unit.empty())
        throw cRuntimeError("Attempt to use the value '%s' as a dimensionless number", str().c_str());
    return intv;
}

intval_t cValue::intValueRaw() const
{
    if (type != INT)
        cannotCastError(INT);
    return intv;
}

inline double safeCastToDouble(intval_t x)
{
    double d = (double)x;
    intval_t x2 = (intval_t)d;
    if (x != x2)
        throw cRuntimeError("Integer %" PRId64 " too large, conversion to double would incur precision loss (hint: if this occurs in NED or ini, use the double() operator to suppress this error)", (int64_t)x);
    return d;
}

void cValue::convertToDouble()
{
    if (type == INT) {
        type = DOUBLE;
        dbl = safeCastToDouble(intv);
    }
    else if (type != DOUBLE)
        cannotCastError(DOUBLE);
}

inline const char *emptyToNone(const char *s) { return (s && *s) ? s : "none"; }

intval_t cValue::intValueInUnit(const char *targetUnit) const
{
    if (type == INT) {
        double c = UnitConversion::getConversionFactor(getUnit(), targetUnit);
        if (c == 1)
            return intv;
        else if (c > 1 && c == floor(c))
            return safeMul((intval_t)c, intv);
        else
            throw cRuntimeError("Cannot convert integer from unit %s to %s: no conversion or conversion rate is not integer",
                    emptyToNone(getUnit()), emptyToNone(targetUnit));
    }
    else {
        cannotCastError(INT);
    }
}

double cValue::doubleValue() const
{
    if (type != DOUBLE && type != INT)
        cannotCastError(DOUBLE);
    if (!unit.empty())
        throw cRuntimeError("Attempt to use the value '%s' as a dimensionless number", str().c_str());
    return type == DOUBLE ? dbl : intv;
}

double cValue::doubleValueRaw() const
{
    if (type == DOUBLE)
        return dbl;
    else if (type == INT)
        return intv;
    else
        cannotCastError(DOUBLE);
}

double cValue::doubleValueInUnit(const char *targetUnit) const
{
    if (type == DOUBLE)
        return UnitConversion::convertUnit(dbl, unit.c_str(), targetUnit);
    else if (type == INT)
        return UnitConversion::convertUnit(safeCastToDouble(intv), unit.c_str(), targetUnit);
    else
        cannotCastError(DOUBLE);
}

void cValue::convertTo(const char *targetUnit)
{
    assertType(DOUBLE);
    dbl = UnitConversion::convertUnit(dbl, unit.c_str(), targetUnit);
    unit = targetUnit;
}

void cValue::setUnit(const char* unit)
{
    if (type != DOUBLE && type != INT)
        throw cRuntimeError("Cannot set measurement unit on a value of type %s", getTypeName(type));
    this->unit = unit;
}

bool cValue::containsObject() const
{
    return getContainedObject(this) != nullptr;
}

bool cValue::containsXML() const
{
    return dynamic_cast<cXMLElement*>(getContainedObject(this)) != nullptr;
}

cObject *cValue::objectValue() const
{
    assertType(POINTER);
    return fromAnyPtr<cObject>(ptr);
}

cXMLElement *cValue::xmlValue() const
{
    return check_and_cast_nullable<cXMLElement*>(objectValue());
}

cObject *cValue::getContainedObject(const cValue *p)
{
    return p->type==cValue::POINTER && p->ptr.contains<cObject>() ? fromAnyPtr<cObject>(p->ptr) : nullptr; //TODO ptr.contains<cObject> is not entirely correct
}

double cValue::convertUnit(double d, const char *unit, const char *targetUnit)
{
    // not inline because simkernel header files cannot refer to common/ headers (unitconversion.h)
    return UnitConversion::convertUnit(d, unit, targetUnit);
}

double cValue::parseQuantity(const char *str, const char *expectedUnit)
{
    return UnitConversion::parseQuantity(str, expectedUnit);
}

double cValue::parseQuantity(const char *str, std::string& outActualUnit)
{
    return UnitConversion::parseQuantity(str, outActualUnit);
}

static std::string objectInfo(const cObject *obj)
{
    bool valueLike = dynamic_cast<const cValueMap *>(obj) || dynamic_cast<const cValueArray *>(obj);
    return obj==nullptr ? "nullptr" : valueLike ? obj->str() : obj->getClassAndFullPath();
}

std::string cValue::str() const
{
    char buf[32];
    switch (type) {
        case UNDEF: return "undefined";
        case BOOL: return bl ? "true" : "false";
        case INT: sprintf(buf, "%" PRId64 "%s", (int64_t)intv, opp_nulltoempty(unit.c_str())); return buf;
        case DOUBLE: {
            if (unit.empty()) {
                opp_dtoa(buf, "%g", dbl);
            }
            else {
                double value = dbl;
                const char *displayUnit = unit.c_str();
                if (value < 0.1 || value >= 10000) {
                    displayUnit = UnitConversion::getBestUnit(value, displayUnit);
                    value = UnitConversion::convertUnit(value, unit.c_str(), displayUnit);
                }
                opp_dtoa(buf, "%g", value);
                if (!std::isfinite(value))
                    strcat(buf, " ");
                strcat(buf, displayUnit);
            }
            return buf;
        }
        case STRING: return opp_quotestr(s);
        case POINTER: return ptr.contains<cObject>() ? objectInfo(ptr.get<cObject>()) : ptr.str();
        default: throw cRuntimeError("Internal error: Invalid cValue type");
    }
}

bool cValue::operator==(const cValue& other)
{
    if (type != other.type)
        return false;  // note: no implicit int <--> double conversion

    switch (type) {
        case UNDEF: return true;
        case BOOL: return bl == other.bl;
        case INT: return intv == other.intv && opp_strcmp(unit.c_str(), other.unit.c_str()) == 0;
        case DOUBLE: return dbl == other.dbl && opp_strcmp(unit.c_str(), other.unit.c_str()) == 0;
        case STRING: return s == other.s;
        case POINTER: return ptr == other.ptr; // same object
        default: throw cRuntimeError("Internal error: Invalid cValue type");
    }
}

}  // namespace omnetpp

#include "common/exprvalue.h"

namespace omnetpp {

static std::string printerFunction(common::any_ptr ptr) { return ptr.contains<cObject>() ? objectInfo(ptr.get<cObject>()) : ptr.str(); }
EXECUTE_ON_STARTUP(common::expression::ExprValue::printerFunction = printerFunction);

}  // namespace omnetpp

