//=========================================================================
//  SCAVEEXCEPTION.CC - part of
//                  OMNeT++/OMNEST
//           Discrete System Simulation in C++
//
//=========================================================================

/*--------------------------------------------------------------*
  Copyright (C) 1992-2005 Andras Varga

  This file is distributed WITHOUT ANY WARRANTY. See the file
  `license' for details on this and other legal matters.
*--------------------------------------------------------------*/

#include <sstream>
#include "scaveexception.h"

USING_NAMESPACE


ResultFileFormatException::ResultFileFormatException(const char *message, const char *file, int64 line, file_offset_t offset)
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
