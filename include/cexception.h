//==========================================================================
//  CEXCEPTION.H - part of
//                     OMNeT++/OMNEST
//            Discrete System Simulation in C++
//
//
//  Exception class
//
//==========================================================================

/*--------------------------------------------------------------*
  Copyright (C) 1992-2005 Andras Varga

  This file is distributed WITHOUT ANY WARRANTY. See the file
  `license' for details on this and other legal matters.
*--------------------------------------------------------------*/

#ifndef __CEXCEPTION_H
#define __CEXCEPTION_H

#include <stdarg.h>
#include "defs.h"
#include "errmsg.h"
#include "opp_string.h"

class cPolymorphic;
class cComponent;

/**
 * Exception class.
 *
 * @ingroup SimSupport
 */
class SIM_API cException
{
  protected:
    int errorcode;
    opp_string msg;
    bool hascontext;
    opp_string contextclassname;
    opp_string contextfullpath;
    int moduleid;

    /**
     * Helper function for constructors: assembles and stores the message text.
     * If the first arg is non-NULL, the message text will be prepended (if needed)
     * with the object type and name, like this: "(cArray)array: ..."
     */
    void init(const cPolymorphic *obj, ErrorCode errorcode, const char *fmt, va_list va);

    // helper for init()
    void storeCtx();

    // default constructor, for subclasses only.
    cException();

    //
    // Helper, called from cException constructors.
    //
    // If an exception occurs in initialization code (during construction of
    // global objects, before main() is called), there's nobody who could
    // catch the error, so it would just cause a program abort.
    // Here we handle this case manually: if cException ctor is invoked before
    // main() has started, we print the error message and call exit(1).
    //
    void exitIfStartupError();

  public:
    /** @name Constructors, destructor */
    //@{
    /**
     * Error is identified by an error code, and the message comes from a
     * string table. The error string may expect printf-like arguments
     * (%s, %d) which also have to be passed to the constructor.
     */
    cException(ErrorCode errcode,...);

    /**
     * To be called like printf(). The error code is set to eCUSTOM.
     */
    cException(const char *msg,...);

    /**
     * Error is identified by an error code, and the message comes from a
     * string table. The error string may expect printf-like arguments
     * (%s, %d) which also have to be passed to the constructor.
     * The 1st arg is the object where the error occurred: its class and
     * object name will be prepended to the message like this: "(cArray)arr".
     */
    cException(const cPolymorphic *where, ErrorCode errcode,...);

    /**
     * To be called like printf(). The error code is set to eCUSTOM.
     * The 1st arg is the object where the error occurred: its class and
     * object name will be prepended to the message like this: "(cArray)arr".
     */
    cException(const cPolymorphic *where, const char *msg,...);

    /**
     * Destructor.
     */
    virtual ~cException() {}
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
    const char *message() {return msg.c_str();}

    /**
     * Returns true if the exception has "context info", that is, it occurred
     * within a known module or channel. contextClassName() and contextFullPath()
     * and moduleID() can only be called if this method returned true.
     */
    bool hasContext() {return hascontext;}

    /**
     * Returns the class name (NED type name) of the context where the
     * exception occurred.
     */
    const char *contextClassName() {return contextclassname.c_str();}

    /**
     * Returns the full path of the context where the exception
     * occurred.
     */
    const char *contextFullPath() {return contextfullpath.c_str();}

    /**
     * Returns the ID of the module where the exception occurred,
     * or -1 if it was not inside a module. The module may not exist
     * any more when the exception is caught (ie. if the exception occurs
     * during network setup, the network is cleaned up immediately).
     */
    int moduleID() {return moduleid;}   // FIXME check all code that calls this!!! maybe they need hasContext() instead
    //@}
};

/**
 * Thrown when the simulation is completed.
 * For example, cSimpleModule::endSimulation() throws this exception.
 * Statistics object may also throw this exception to
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
    cTerminationException(ErrorCode errcode,...);

    /**
     * To be called like printf(). The error code is set to eCUSTOM.
     */
    cTerminationException(const char *msg,...);
};

/**
 * Thrown when the simulation kernel or other components detect a runtime
 * error. For example, cSimpleModule::scheduleAt() throws this exception when
 * the specified simulation time is in the past, or the message pointer
 * is NULL.
 *
 * @ingroup Internals
 */
class SIM_API cRuntimeError : public cException
{
  protected:
    // internal
    void breakIntoDebuggerIfRequested();

  public:
    /**
     * Error is identified by an error code, and the message comes from a
     * string table. The error string may expect printf-like arguments
     * (%s, %d) which also have to be passed to the constructor.
     */
    cRuntimeError(ErrorCode errcode,...);

    /**
     * To be called like printf(). The error code is set to eCUSTOM.
     */
    cRuntimeError(const char *msg,...);

    /**
     * Error is identified by an error code, and the message comes from a
     * string table. The error string may expect printf-like arguments
     * (%s, %d) which also have to be passed to the constructor.
     * The 1st arg is the object where the error occurred: its class and
     * object name will be prepended to the message like this: "(cArray)arr".
     */
    cRuntimeError(const cPolymorphic *where, ErrorCode errcode,...);

    /**
     * To be called like printf(). The error code is set to eCUSTOM.
     * The 1st arg is the object where the error occurred: its class and
     * object name will be prepended to the message like this: "(cArray)arr".
     */
    cRuntimeError(const cPolymorphic *where, const char *msg,...);
};

/**
 * This exception is only thrown from cModule::end(), and from deleteModule()
 * if the current module is to be deleted, in order to exit that module
 * immediately.
 *
 * @ingroup Internals
 */
class SIM_API cEndModuleException : public cException
{
  private:
    bool del; // if true, currently running module should be deleted

  public:
    /**
     * Constructor. If the bool argument is true, the module should be deleted.
     */
    cEndModuleException(bool moduleToBeDeleted=false) {del = moduleToBeDeleted;}

    /**
     * If true, the module should be deleted.
     */
    bool moduleToBeDeleted() {return del;}
};

/**
 * Used internally when deleting an activity() simple module.
 * Then, the coroutine running activity() is "asked" to throw a
 * cStackCleanupException to achieve stack unwinding, a side effect of
 * exceptions, in order to properly clean up activity()'s local variables.
 *
 * @ingroup Internals
 */
class SIM_API cStackCleanupException : public cException
{
  public:
    /**
     * Constructor.
     */
    cStackCleanupException();
};

#endif
