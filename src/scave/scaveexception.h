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
#include "commondefs.h"
#include "scavedefs.h"
#include "exception.h"

/**
 * Exception thrown when a syntax error found while reading a result file.
 */
class SCAVE_API ResultFileFormatException : public opp_runtime_error
{
  private:
      std::string file;
      int line;
      long offset;

  public:
    /**
     * Constructor with error message and location.
     */
    ResultFileFormatException(const char *msg, const char *file, int line, long offset = -1);

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
    int getLine() const { return line; }
    
    /**
     * Returns the file offset at which the error occured.
     */
    long getFileOffset() const { return offset; }
};

#endif


