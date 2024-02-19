//==========================================================================
//  EXPRVALUE.CC  - part of
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

#include <cmath>
#include <cinttypes>  // PRId64
#include "pooledstring.h"
#include "unitconversion.h"
#include "exception.h"
#include "exprvalue.h"

namespace omnetpp {
namespace common {
namespace expression {

std::string (*ExprValue::printerFunction)(any_ptr ptr);

void ExprValue::operator=(const ExprValue& other)
{
    deleteOld();
    type = other.type;
    switch (type) {
        case UNDEF: break;
        case BOOL: bl = other.bl; break;
        case INT: intv = other.intv; unit = other.unit; break;
        case DOUBLE: dbl = other.dbl; unit = other.unit; break;
        case STRING: s = strdup(other.s); break;
        case POINTER: ptr = other.ptr; break;
    }
}

void ExprValue::operator=(ExprValue&& other)
{
    deleteOld();
    type = other.type;
    switch (type) {
        case UNDEF: break;
        case BOOL: bl = other.bl; break;
        case INT: intv = other.intv; unit = other.unit; break;
        case DOUBLE: dbl = other.dbl; unit = other.unit; break;
        case STRING: s = other.s; other.type = UNDEF; other.s = nullptr; break;
        case POINTER: ptr = other.ptr; break;
    }
}

const char *ExprValue::getTypeName(Type t)
{
    switch (t) {
        case UNDEF:  return "undefined";
        case BOOL:   return "bool";
        case INT:    return "integer";
        case DOUBLE: return "double";
        case STRING: return "string";
        case POINTER: return "object"; // kinda confusing, I know -- but it needs to be consistent with NED param type, NED function arg type etc.
        default:     return "<unknown>";
    }
}

void ExprValue::cannotCastError(Type targetType) const
{
    const char *note = (getType() == DOUBLE && targetType==INT) ?
            " (note: no implicit conversion from double to int)" : "";
    throw opp_runtime_error("Cannot cast '%s' from type '%s' to '%s'%s",
            str().c_str(), getTypeName(type), getTypeName(targetType), note);
}

intval_t ExprValue::intValue() const
{
    if (type == INT)
        return intv;
    else
        cannotCastError(INT);
}

void ExprValue::convertToDouble()
{
    if (type == INT) {
        type = DOUBLE;
        dbl = safeCastToDouble(intv);
    }
    else if (type != DOUBLE)
        cannotCastError(DOUBLE);
}

inline const char *emptyToNone(const char *s) { return (s && *s) ? s : "none"; }

intval_t ExprValue::intValueInUnit(const char *targetUnit) const
{
    if (type == INT) {
        double c = UnitConversion::getConversionFactor(getUnit(), targetUnit);
        if (c == 1)
            return intv;
        else if (c > 1 && c == floor(c))
            return safeMul((intval_t)c, intv);
        else
            throw opp_runtime_error("Cannot convert integer from unit '%s' to '%s': no conversion or conversion rate is not integer",
                    emptyToNone(getUnit()), emptyToNone(targetUnit));
    }
    else {
        cannotCastError(INT);
    }
}

double ExprValue::doubleValue() const
{
    if (type == DOUBLE)
        return dbl;
    else if (type == INT)
        return intv;
    else
        cannotCastError(DOUBLE);
    return 0;
}

double ExprValue::doubleValueInUnit(const char *targetUnit) const
{
    if (type == DOUBLE)
        return UnitConversion::convertUnit(dbl, unit.c_str(), targetUnit);
    else if (type == INT)
        return UnitConversion::convertUnit(safeCastToDouble(intv), unit.c_str(), targetUnit);
    else
        cannotCastError(DOUBLE);
    return 0;
}

void ExprValue::convertTo(const char *targetUnit)
{
    ensureType(DOUBLE);
    dbl = UnitConversion::convertUnit(dbl, unit.c_str(), targetUnit);
    unit = targetUnit;
}

void ExprValue::setUnit(const char *unit)
{
    if (type != DOUBLE && type != INT)
        throw opp_runtime_error("Cannot set measurement unit on a value of type '%s'", getTypeName(type));
    this->unit = unit;
}

std::string ExprValue::str() const
{
    char buf[64];
    switch (type) {
        case UNDEF:
            return "undefined";
        case BOOL:
            return bl ? "true" : "false";
        case INT:
            snprintf(buf, sizeof(buf), "%" PRId64 "%s", (int64_t)intv, opp_nulltoempty(unit.c_str()));
            return buf;
        case DOUBLE:
            opp_dtoa(buf, "%g", dbl);
            if (!unit.empty()) {
                if (!std::isfinite(dbl))
                    strcat(buf, " ");
                strcat(buf, unit.c_str());
            }
            return buf;
        case STRING:
            return opp_quotestr(s);
        case POINTER:
            return printerFunction ? printerFunction(ptr) : ptr.str();
        default:
            throw opp_runtime_error("Internal error: Invalid ExprValue type");
    }
}

}  // namespace expression
}  // namespace common
}  // namespace omnetpp

