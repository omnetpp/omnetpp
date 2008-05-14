//=========================================================================
//  EXCEPTION.H - part of
//                  OMNeT++/OMNEST
//           Discrete System Simulation in C++
//
//  Author: Andras Varga
//
//=========================================================================

/*--------------------------------------------------------------*
  Copyright (C) 2006-2008 OpenSim Ltd.

  This file is distributed WITHOUT ANY WARRANTY. See the file
  `license' for details on this and other legal matters.
*--------------------------------------------------------------*/

#ifndef _EXCEPTION_H_
#define _EXCEPTION_H_

#include <string>
#include <exception>
#include <stdexcept>
#include "commondefs.h"

NAMESPACE_BEGIN

//FIXME rename this file

/**
 * Utility class: makes std::runtime_error somewhat more convenient to use,
 * by providing a printf-style constructor. Catch as std::runtime_error.
 */
class COMMON_API opp_runtime_error : public std::runtime_error
{
  protected:
    std::string errormsg;

  public:
    /**
     * The error message can be generated in a printf-like manner.
     */
    opp_runtime_error(const char *msg,...);

    /**
     * Destructor with throw clause required by gcc.
     */
    virtual ~opp_runtime_error() throw() {}

    /**
     * Returns the text of the error. Redefined from std::exception.
     */
    virtual const char *what() const throw() {return errormsg.c_str();}
};

NAMESPACE_END


#endif


