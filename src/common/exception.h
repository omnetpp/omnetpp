//=========================================================================
//  EXCEPTION.H - part of
//                  OMNeT++/OMNEST
//           Discrete System Simulation in C++
//
//  Author: Andras Varga
//
//=========================================================================

/*--------------------------------------------------------------*
  Copyright (C) 2006-2017 OpenSim Ltd.

  This file is distributed WITHOUT ANY WARRANTY. See the file
  `license' for details on this and other legal matters.
*--------------------------------------------------------------*/

#ifndef __OMNETPP_COMMON_EXCEPTION_H
#define __OMNETPP_COMMON_EXCEPTION_H

#include <string>
#include <exception>
#include <stdexcept>
#include "commondefs.h"

namespace omnetpp {
namespace common {

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
    virtual const char *what() const throw() override {return errormsg.c_str();}
};

} // namespace common
}  // namespace omnetpp


#endif


