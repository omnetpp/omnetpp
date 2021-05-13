//==========================================================================
// exception.h  -
//
//                     OMNeT++/OMNEST
//            Discrete System Simulation in C++
//
//==========================================================================

/*--------------------------------------------------------------*
  Copyright (C) 2002-2017 Andras Varga
  Copyright (C) 2006-2017 OpenSim Ltd.

  This file is distributed WITHOUT ANY WARRANTY. See the file
  `license' for details on this and other legal matters.
*--------------------------------------------------------------*/

#ifndef __OMNETPP_NEDXML_EXCEPTION_H
#define __OMNETPP_NEDXML_EXCEPTION_H

#include <string>
#include <exception>
#include <stdexcept>
#include "nedxmldefs.h"

namespace omnetpp {
namespace nedxml {

class ASTNode;

/**
 * @brief Low-level routines throw an exception instead of calling NedErrorStore->add().
 */
class NEDXML_API NedException : public std::runtime_error
{
  protected:
    std::string msg;

  public:
    /**
     * The error message can be generated in a printf-like manner.
     */
    _OPP_GNU_ATTRIBUTE(format(printf, 2, 3))
    NedException(const char *messagefmt,...);

    /**
     * The error message can be generated in a printf-like manner.
     */
    _OPP_GNU_ATTRIBUTE(format(printf, 3, 4))
    NedException(ASTNode *context, const char *messagefmt,...);

    /**
     * Empty destructor with throw clause for gcc.
     */
    virtual ~NedException() throw() {}

    /**
     * Returns the text of the error. Redefined from std::exception.
     */
    virtual const char *what() const throw() override {return msg.c_str();}
};


}  // namespace nedxml
}  // namespace omnetpp


#endif

