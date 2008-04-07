//=========================================================================
//  SCAVEEXCEPTION.H - part of
//                  OMNeT++/OMNEST
//           Discrete System Simulation in C++
//
//=========================================================================

/*--------------------------------------------------------------*
  Copyright (C) 1992-2005 Andras Varga

  This file is distributed WITHOUT ANY WARRANTY. See the file
  `license' for details on this and other legal matters.
*--------------------------------------------------------------*/

#ifndef _SCAVEEXCEPTION_H_
#define _SCAVEEXCEPTION_H_

#include <string>
#include <exception>
#include <stdexcept>
#include "platmisc.h"
#include "commondefs.h"
#include "scavedefs.h"
#include "exception.h"

NAMESPACE_BEGIN

/**
 * Exception thrown when a syntax error found while reading a result file.
 */
class SCAVE_API ResultFileFormatException : public opp_runtime_error
{
  private:
      std::string file;
      int64 line;
      file_offset_t offset;

  public:
    /**
     * Constructor with error message and location.
     */
    ResultFileFormatException(const char *msg, const char *file, int64 line, file_offset_t offset = -1);

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
    int64 getLine() const { return line; }
    
    /**
     * Returns the file offset at which the error occured.
     */
    file_offset_t getFileOffset() const { return offset; }
};

NAMESPACE_END


#endif


