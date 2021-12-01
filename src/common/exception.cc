//=========================================================================
//  EXCEPTION.CC - part of
//                  OMNeT++/OMNEST
//           Discrete System Simulation in C++
//
//=========================================================================

/*--------------------------------------------------------------*
  Copyright (C) 2006-2017 OpenSim Ltd.

  This file is distributed WITHOUT ANY WARRANTY. See the file
  `license' for details on this and other legal matters.
*--------------------------------------------------------------*/

#include <cstdio>
#include <cstdarg>
#include "exception.h"
#include "commonutil.h"

namespace omnetpp {
namespace common {


opp_runtime_error::opp_runtime_error(const char *messagefmt, ...) : std::runtime_error("")
{
    char buf[1024];
    VSNPRINTF(buf, 1024, messagefmt);
    errormsg = buf;
}

}  // namespace common
}  // namespace omnetpp

