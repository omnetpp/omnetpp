//=========================================================================
//  SCAVEEXCEPTION.CC - part of
//                  OMNeT++/OMNEST
//           Discrete System Simulation in C++
//
//=========================================================================

/*--------------------------------------------------------------*
  Copyright (C) 2006-2017 OpenSim Ltd.

  This file is distributed WITHOUT ANY WARRANTY. See the file
  `license' for details on this and other legal matters.
*--------------------------------------------------------------*/

#include <sstream>
#include "scaveexception.h"

namespace omnetpp {
namespace scave {


ResultFileFormatException::ResultFileFormatException(const char *message, const char *file, int64_t line, file_offset_t offset)
    : opp_runtime_error(""), file(file), line(line), offset(offset)
{
    std::ostringstream msg;
    msg << message << ", file " << file;
    if (line >= 0)
        msg << ", line " << line;
    if (offset >= 0)
        msg << ", offset " << offset;

    errormsg = msg.str();
}

}  // namespace scave
}  // namespace omnetpp

