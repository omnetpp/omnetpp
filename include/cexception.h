//==========================================================================
//   CEXCEPTION.H - header for
//                             OMNeT++
//            Discrete System Simulation in C++
//
//
//  Exception class
//
//==========================================================================

/*--------------------------------------------------------------*
  Copyright (C) 1992-2002 Andras Varga
  Technical University of Budapest, Dept. of Telecommunications,
  Stoczek u.2, H-1111 Budapest, Hungary.

  This file is distributed WITHOUT ANY WARRANTY. See the file
  `license' for details on this and other legal matters.
*--------------------------------------------------------------*/

#ifndef __CEXCEPTION_H
#define __CEXCEPTION_H

#include <stdarg.h>  // for va_list
#include "defs.h"
#include "util.h"

class cObject;
class cModule;

/**
 * Exception class.
 */
class SIM_API cException
{
  protected:
    int errorcode;
    opp_string msg;
    opp_string modulefullpath;
    int moduleid;

    /**
     * Helper function for constructors: assembles and stores the message text.
     * If obj is non-NULL, the message text will be prepended with the object
     * type and name, like this: "(cArray)array: ..."
     */
    void init(cObject *obj, int errc, const char *fmt, va_list va);

    // helper for init()
    void storeCtx();

  public:
    /** @name Constructors */
    //@{
    /**
     * Default constructor.
     */
    cException();

    /**
     * Error is identified by an error code, and the message comes from a
     * string table. The error string may expect printf-like arguments
     * (%s, %d) which also have to be passed to the constructor.
     */
    cException(int errcode,...);

    /**
     * To be called like printf(). The error code is set to eCUSTOM.
     */
    cException(const char *msg,...);

    /**
     * Error is identified by an error code, and the message comes from a
     * string table. The error string may expect printf-like arguments
     * (%s, %d) which also have to be passed to the constructor.
     */
    cException(cObject *where, int errcode,...);

    /**
     * To be called like printf(). The error code is set to eCUSTOM.
     */
    cException(cObject *where, const char *msg,...);
    //@}

    /** @name Getting exception info */
    //@{

    /**
     * Returns the error code.
     */
    int errorCode() {return errorcode;}

    /**
     * Returns the text of the error.
     */
    const char *message() {return msg;}

    /**
     * Returns the full path of the module where the exception
     * occurred.
     */
    const char *moduleFullPath() {return modulefullpath;}

    /**
     * Returns the ID of the module where the exception occurred,
     * or -1 if it was not inside a module. The module may not exist
     * any more when the exception is caught (ie. if the exception occurs
     * during network setup, the network is cleaned up immediately).
     */
    int moduleID() {return moduleid;}

//    /**
//     * Tell if this exception signalled an error or a normal termination.
//     */
//    bool isNormalTermination() const;
    //@}
};

/**
 * This exception is thrown when there is no error, but the simulation has
 * to be terminated. For example, cSimpleModule::endSimulation() throws
 * this exception. Statistics object may also use this exception to
 * signal that accuracy of simulation results has reached the desired level.
 *
 * @ingroup Internals
 */
class SIM_API cTerminationException : public cException
{
  public:
    /**
     * Error is identified by an error code, and the message comes from a
     * string table. The error string may expect printf-like arguments
     * (%s, %d) which also have to be passed to the constructor.
     */
    cTerminationException(int errcode,...);

    /**
     * To be called like printf(). The error code is set to eCUSTOM.
     */
    cTerminationException(const char *msg,...);
};

/**
 * This exception is only thrown from cModule::end(), to exit that module
 * immediately. The exception is simply ignored in the even loop,
 * and simulation continues.
 *
 * @ingroup Internals
 */
class SIM_API cEndModuleException : public cException
{
  public:
    /**
     * Constructor.
     */
    cEndModuleException() {}
};

#endif
