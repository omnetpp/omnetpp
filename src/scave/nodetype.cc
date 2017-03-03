//=========================================================================
//  NODETYPE.CC - part of
//                  OMNeT++/OMNEST
//           Discrete System Simulation in C++
//
//  Author: Andras Varga
//
//=========================================================================

/*--------------------------------------------------------------*
  Copyright (C) 1992-2017 Andras Varga
  Copyright (C) 2006-2017 OpenSim Ltd.

  This file is distributed WITHOUT ANY WARRANTY. See the file
  `license' for details on this and other legal matters.
*--------------------------------------------------------------*/

#include "common/commonutil.h"
#include "nodetype.h"

using namespace omnetpp::common;

namespace omnetpp {
namespace scave {

void NodeType::checkAttrNames(const StringMap& attrs) const
{
    StringMap allowedAttrs;
    getAttributes(allowedAttrs);

    StringMap defaultAttrs;
    getAttrDefaults(defaultAttrs);

    // are there illegal attributes?
    for (StringMap::const_iterator it = attrs.begin(); it != attrs.end(); ++it) {
        const char *attr = it->first.c_str();
        StringMap::iterator j = allowedAttrs.find(attr);
        if (j == allowedAttrs.end())
            throw opp_runtime_error("Illegal attribute '%s'", attr);
    }

    // do we have all attributes?
    for (StringMap::const_iterator i = allowedAttrs.begin(); i != allowedAttrs.end(); ++i) {
        const char *attr = i->first.c_str();
        StringMap::const_iterator it = attrs.find(attr);
        if (it == attrs.end()) {
            if (defaultAttrs.find(attr) == defaultAttrs.end())
                throw opp_runtime_error("Missing attribute '%s'", attr);
        }
    }
}

void NodeType::mapVectorAttributes(StringMap& attrs,  /*out*/ StringVector& warnings) const
{
    // identity map by default
}

}  // namespace scave
}  // namespace omnetpp

