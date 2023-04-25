//=========================================================================
//  EXCEPTION.H - part of
//                  OMNeT++/OMNEST
//           Discrete System Simulation in C++
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

    opp_runtime_error() : std::runtime_error("") {}

  public:
    /**
     * Constructor with a printf-like argument list.
     */
    _OPP_GNU_ATTRIBUTE(format(printf, 2, 3))
    opp_runtime_error(const char *msg,...);

    /**
     * String constructor.
     */
    opp_runtime_error(const std::string& msg) : std::runtime_error(""), errormsg(msg) {}

    virtual ~opp_runtime_error() {}

    /**
     * Returns the text of the error. Redefined from std::exception.
     */
    virtual const char *what() const throw() override {return errormsg.c_str();}
};

}  // namespace common
}  // namespace omnetpp


#endif


