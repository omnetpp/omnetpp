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

const char *cNedValue::OVERFLOW_MSG = "cNEDValue: Integer overflow casting %s to a smaller or unsigned integer type";

void cNedValue::operator=(const cNedValue& other)
{
    type = other.type;
    switch (type) {
        case UNDEF: break;
        case BOOL: bl = other.bl; break;
        case DOUBLE: dbl = other.dbl; dblunit = other.dblunit; break;
        case STRING: s = other.s; break;
        case XML: xml = other.xml; break;
    }
}

const char *cNedValue::getTypeName(Type t)
{
    switch (t) {
        case UNDEF:  return "undef";
        case BOOL:   return "bool";
        case DOUBLE: return "double";
        case STRING: return "string";
        case XML:    return "xml";
        default:     return "???";
    }
}

void cNedValue::cannotCastError(Type t) const
{
    throw cRuntimeError("cNedValue: Cannot cast %s from type %s to %s",
            str().c_str(), getTypeName(type), getTypeName(t));
}

void cNedValue::set(const cPar& par)
{
    switch (par.getType()) {
        case cPar::BOOL: *this = par.boolValue(); break;
        case cPar::DOUBLE: *this = par.doubleValue(); dblunit = par.getUnit(); break;
        case cPar::INT: *this = par.doubleValue(); dblunit = par.getUnit(); break;
        case cPar::STRING: *this = par.stdstringValue(); break;
        case cPar::XML: *this = par.xmlValue(); break;
        default: throw cRuntimeError("Internal error: Invalid cPar type: %s", par.getFullPath().c_str());
    }
}

void cNedValue::convertTo(const char *unit)
{
    dbl = convertUnit(dbl, dblunit, unit);
    dblunit = unit;
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
        case DOUBLE: sprintf(buf, "%g%s", dbl, opp_nulltoempty(dblunit)); return buf;
        case STRING: return opp_quotestr(s);
        case XML: return xml->str();
        default: throw cRuntimeError("Internal error: Invalid cNedValue type");
    }
}

}  // namespace omnetpp

