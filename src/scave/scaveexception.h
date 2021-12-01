//=========================================================================
//  SCAVEEXCEPTION.H - part of
//                  OMNeT++/OMNEST
//           Discrete System Simulation in C++
//
//=========================================================================

/*--------------------------------------------------------------*
  Copyright (C) 2006-2017 OpenSim Ltd.

  This file is distributed WITHOUT ANY WARRANTY. See the file
  `license' for details on this and other legal matters.
*--------------------------------------------------------------*/

#ifndef __OMNETPP_SCAVE_SCAVEEXCEPTION_H
#define __OMNETPP_SCAVE_SCAVEEXCEPTION_H

#include <string>
#include <exception>
#include <stdexcept>
#include "common/commondefs.h"
#include "common/exception.h"
#include "omnetpp/platdep/platmisc.h"
#include "scavedefs.h"

namespace omnetpp {
namespace scave {

/**
 * Exception thrown when a syntax error found while reading a result file.
 */
class SCAVE_API ResultFileFormatException : public opp_runtime_error
{
  private:
      std::string file;
      int64_t line;
      file_offset_t offset;

  public:
    /**
     * Constructor with error message and location.
     */
    ResultFileFormatException(const char *msg, const char *file, int64_t line, file_offset_t offset = -1);

    /**
     * Destructor with throw clause required by gcc.
     */
    virtual ~ResultFileFormatException() throw() {}

    /**
     * Returns the name of the file in which the error occured.
     */
    const char *getFileName() const { return file.c_str(); }

    /**
     * Returns the line number at which the error occured.
     */
    int64_t getLine() const { return line; }

    /**
     * Returns the file offset at which the error occured.
     */
    file_offset_t getFileOffset() const { return offset; }
};

}  // namespace scave
}  // namespace omnetpp


#endif


