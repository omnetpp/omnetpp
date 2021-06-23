//========================================================================
//  ANY_PTR.CC - part of
//                 OMNeT++/OMNEST
//              Discrete System Simulation in C++
//
//  Author: Andras Varga
//
//========================================================================

/*--------------------------------------------------------------*
  Copyright (C) 1992-2017 Andras Varga
  Copyright (C) 2006-2017 OpenSim Ltd.

  This file is distributed WITHOUT ANY WARRANTY. See the file
  `license' for details on this and other legal matters.
*--------------------------------------------------------------*/

#include "omnetpp/any_ptr.h"
#include "common/stringutil.h"

namespace omnetpp {

void any_ptr::checkType(const std::type_info& asType) const
{
    if (ptr == nullptr) {
        if (*type != asType && *type != typeid(std::nullptr_t))
            throw cRuntimeError("Attempt to read any_ptr((%s)nullptr) as %s", opp_typename(*type), opp_typename(asType));
    }
    else if (*type != asType) {
        throw cRuntimeError("Attempt to read any_ptr(%s) as %s", opp_typename(*type), opp_typename(asType));
    }
}

std::string any_ptr::str() const
{
    return common::opp_stringf("(%s)%p", opp_typename(*type), ptr);
}


}  // namespace omnetpp

