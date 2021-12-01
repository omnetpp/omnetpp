//========================================================================
//  ANY_PTR.CC - part of
//                 OMNeT++/OMNEST
//              Discrete System Simulation in C++
//
//========================================================================

/*--------------------------------------------------------------*
  Copyright (C) 1992-2017 Andras Varga
  Copyright (C) 2006-2017 OpenSim Ltd.

  This file is distributed WITHOUT ANY WARRANTY. See the file
  `license' for details on this and other legal matters.
*--------------------------------------------------------------*/

#include "any_ptr.h"
#include "stringutil.h"
#include "exception.h"

namespace omnetpp {
namespace common {

std::string (*any_ptr::printerFunction)(any_ptr ptr);

void any_ptr::checkType(const std::type_info& asType) const
{
    if (ptr == nullptr) {
        if (*type != asType && *type != typeid(std::nullptr_t))
            throw opp_runtime_error("Attempt to read any_ptr((%s)nullptr) as %s", opp_typename(*type), opp_typename(asType));
    }
    else if (*type != asType) {
        throw opp_runtime_error("Attempt to read any_ptr(%s) as %s", opp_typename(*type), opp_typename(asType));
    }
}

std::string any_ptr::str() const
{
    if (printerFunction)
        return printerFunction(*this);
    else if (ptr == nullptr)
        return "nullptr";
    else
        return opp_stringf("(%s)%p", opp_typename(*type), ptr);
}

}  // namespace common
}  // namespace omnetpp

