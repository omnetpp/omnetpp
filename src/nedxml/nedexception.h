//==========================================================================
// nedexception.h  -
//
//                     OMNeT++/OMNEST
//            Discrete System Simulation in C++
//
//==========================================================================

/*--------------------------------------------------------------*
  Copyright (C) 2002-2008 Andras Varga
  Copyright (C) 2006-2008 OpenSim Ltd.

  This file is distributed WITHOUT ANY WARRANTY. See the file
  `license' for details on this and other legal matters.
*--------------------------------------------------------------*/

#ifndef __NEDEXCEPTION_H
#define __NEDEXCEPTION_H

#include <string>
#include <exception>
#include <stdexcept>
#include "nedxmldefs.h"

NAMESPACE_BEGIN

class NEDElement;

/**
 * Low-level routines throw an exception instead of calling NEDErrorStore->add().
 */
class NEDXML_API NEDException : public std::runtime_error
{
  protected:
    std::string errormsg;

  public:
    /**
     * The error message can be generated in a printf-like manner.
     */
    NEDException(const char *messagefmt,...);

    /**
     * The error message can be generated in a printf-like manner.
     */
    NEDException(NEDElement *context, const char *messagefmt,...);

    /**
     * Empty desctructor with throw clause for gcc.
     */
    virtual ~NEDException() throw() {}

    /**
     * Returns the text of the error. Redefined from std::exception.
     */
    virtual const char *what() const throw() {return errormsg.c_str();}
};


NAMESPACE_END


#endif

