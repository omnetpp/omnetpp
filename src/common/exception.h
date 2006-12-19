//=========================================================================
//  EXCEPTION.H - part of
//                  OMNeT++/OMNEST
//           Discrete System Simulation in C++
//
//=========================================================================

/*--------------------------------------------------------------*
  Copyright (C) 1992-2005 Andras Varga

  This file is distributed WITHOUT ANY WARRANTY. See the file
  `license' for details on this and other legal matters.
*--------------------------------------------------------------*/

#ifndef _EXCEPTION_H_
#define _EXCEPTION_H_

#include <string>
#include <exception>
#include <stdexcept>

/**
 * Exceptions of this type are thrown on errors during processing.
 */
class Exception : public std::runtime_error //FIXME use some more specific name...
{
  protected:
    std::string errormsg;

  public:
    /**
     * The error message can be generated in a printf-like manner.
     */
    Exception(const char *msg,...);

    /**
     * Returns the text of the error. Redefined from std::exception.
     */
    virtual const char *what() {return errormsg.c_str();}
};


#endif


