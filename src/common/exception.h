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

//FIXME rename this file

/**
 * Utility class: makes std::runtime_error somewhat more convenient to use,
 * by providing a printf-style constructor. Catch as std::runtime_error.
 */
class opp_runtime_error : public std::runtime_error
{
  protected:
    std::string errormsg;

  public:
    /**
     * The error message can be generated in a printf-like manner.
     */
    opp_runtime_error(const char *msg,...);

    /**
     * Returns the text of the error. Redefined from std::exception.
     */
    virtual const char *what() {return errormsg.c_str();}
};

#endif


