//=========================================================================
//  CWATCH.CC - part of
//
//                  OMNeT++/OMNEST
//           Discrete System Simulation in C++
//
//
//   Member functions of
//    cWatchBase etc: make primitive types, structs etc inspectable
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

#include "common/stringutil.h"
#include "omnetpp/cwatch.h"

using namespace omnetpp::common;

namespace omnetpp {

std::string cWatch_stdstring::str() const
{
    return opp_quotestr(r);
}

void cWatch_stdstring::assign(const char *s)
{
    if (s[0] == '"' && s[strlen(s)-1] == '"') {
        r = opp_parsequotedstr(s);
    }
    else {
        r = s;
    }
}

}  // namespace omnetpp

