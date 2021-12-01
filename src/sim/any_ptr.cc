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

#include "omnetpp/any_ptr.h"
#include "omnetpp/cobject.h"
#include "omnetpp/onstartup.h"
#include "common/stringutil.h"
#include "common/any_ptr.h"
#include "common/pooledstring.h"

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

const char *any_ptr::typeName() const
{
    std::string s = pointerTypeName();
    s = common::opp_trim(s.substr(0, s.size()-1));
    return common::opp_staticpooledstring::get(s.c_str());
}

std::string any_ptr::str() const
{
    if (ptr == nullptr)
        return "nullptr";
    else if (contains<cObject>())
        return get<cObject>()->getClassAndFullPath();
    else
        return common::opp_stringf("(%s)%p", opp_typename(*type), ptr);
}

static std::string printerFunction(common::any_ptr p) {return any_ptr(p.raw(),p.pointerType()).str();}
EXECUTE_ON_STARTUP(common::any_ptr::printerFunction = printerFunction);


}  // namespace omnetpp

