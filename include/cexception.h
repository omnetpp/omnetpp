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
  Copyright (C) 1992-2008 Andras Varga
  Copyright (C) 2006-2008 OpenSim Ltd.

  This file is distributed WITHOUT ANY WARRANTY. See the file
  `license' for details on this and other legal matters.
*--------------------------------------------------------------*/

#ifndef __CEXCEPTION_H
#define __CEXCEPTION_H

#include <stdarg.h>
#include <exception>
#include <stdexcept>
#include "simkerneldefs.h"
#include "simtime_t.h"
#include "errmsg.h"
#include "opp_string.h"

NAMESPACE_BEGIN

class cObject;
class cComponent;

/**
 * Exception class.
 *
 * @ingroup SimSupport
 */
class SIM_API cException : public std::exception
{
  protected:
    int errorcode;
    std::string msg;

    int simulationstage;
    eventnumber_t eventnumber;
    simtime_t simtime;

    bool hascontext;
    std::string contextclassname;
    std::string contextfullpath;
    int moduleid;

    /**
     * Helper function for constructors: assembles and stores the message text.
     * If the first arg is non-NULL, the message text will be prepended (if needed)
     * with the object type and name, like this: "(cArray)array: ..."
     */
    void init(const cObject *obj, OppErrorCode errorcode, const char *fmt, va_list va);

    // helper for init()
    void storeCtx();

    // default constructor, for subclasses only.
    cException();

    //
    // Helper, called from cException constructors.
    //
    // If an exception occurs in initialization code (during construction of
    // global objects, before main() is called), there is nobody who could
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
    cException(OppErrorCode errcode,...);

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
    cException(const cObject *where, OppErrorCode errcode,...);

    /**
     * To be called like printf(). The error code is set to eCUSTOM.
     * The 1st arg is the object where the error occurred: its class and
     * object name will be prepended to the message like this: "(cArray)arr".
     */
    cException(const cObject *where, const char *msg,...);

    /**
     * We unfortunately need to copy exception objects when handing them back
     * from an activity().
     */
    cException(const cException&);

    /**
     * Virtual copy constructor. We unfortunately need to copy exception objects
     * when handing them back from an activity().
     */
    virtual cException *dup() const {return new cException(*this);}

    /**
     * Destructor.
     */
    virtual ~cException() throw() {}
    //@}

    /** @name Updating the exception message */
    //@{
    /**
     * Overwrites the message text with the given one.
     */
    virtual void setMessage(const char *txt) {msg = txt;}

    /**
     * Prefixes the message with the given text and a colon.
     */
    virtual void prependMessage(const char *txt) {msg = std::string(txt) + ": " + msg;}
    //@}

    /** @name Getting exception info */
    //@{
    /**
     * Whether the exception represents an error or a normal (non-error)
     * terminating condition (e.g. "Simulation completed").
     */
    virtual bool isError() const {return true;}

    /**
     * Returns the error code.
     */
    virtual int getErrorCode() const {return errorcode;}

    /**
     * Returns the text of the error. Redefined from std::exception.
     */
    virtual const char *what() const throw() {return msg.c_str();}

    /**
     * Returns a formatted message that includes the "Error" word, context
     * information, the event number and simulation time if available
     * and relevant, in addition to the exception message (what()).
     */
    virtual std::string getFormattedMessage() const;

    /**
     * Returns in which stage of the simulation the exception object was created:
     * during network building (CTX_BUILD), network initialization (CTX_INITIALIZE),
     * simulation execution (CTX_EVENT), finalization (CTX_FINISH), or network
     * cleanup (CTX_CLEANUP).
     */
    virtual int getSimulationStage() const {return simulationstage;}

    /**
     * Returns the event number at the creation of the exception object.
     */
    virtual eventnumber_t getEventNumber() const {return eventnumber;}

    /**
     * Returns the simulation time at the creation of the exception object.
     */
    virtual simtime_t getSimtime() const {return simtime;}

    /**
     * Returns true if the exception has "context info", that is, it occurred
     * within a known module or channel. getContextClassName(), getContextFullPath()
     * and getModuleID() may only be called if this method returns true.
     */
    virtual bool hasContext() const {return hascontext;}

    /**
     * Returns the class name (NED type name) of the context where the
     * exception occurred.
     */
    virtual const char *getContextClassName() const {return contextclassname.c_str();}

    /**
     * Returns the full path of the context where the exception
     * occurred.
     */
    virtual const char *getContextFullPath() const {return contextfullpath.c_str();}

    /**
     * Returns the ID of the module where the exception occurred,
     * or -1 if it was not inside a module. The module may not exist
     * any more when the exception is caught (ie. if the exception occurs
     * during network setup, the network is cleaned up immediately).
     */
    virtual int getModuleID() const {return moduleid;}
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
    cTerminationException(OppErrorCode errcode,...);

    /**
     * To be called like printf(). The error code is set to eCUSTOM.
     */
    cTerminationException(const char *msg,...);

    /**
     * We unfortunately need to copy exception objects when handing them back
     * from an activity().
     */
    cTerminationException(const cTerminationException& e) : cException(e) {}

    /**
     * Virtual copy constructor. We unfortunately need to copy exception objects
     * when handing them back from an activity().
     */
    virtual cTerminationException *dup() const {return new cTerminationException(*this);}

    /**
     * Termination exceptions are generally not errors, but messages like
     * "Simulation completed".
     */
    virtual bool isError() const {return false;}
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
    cRuntimeError(OppErrorCode errcode,...);

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
    cRuntimeError(const cObject *where, OppErrorCode errcode,...);

    /**
     * To be called like printf(). The error code is set to eCUSTOM.
     * The 1st arg is the object where the error occurred: its class and
     * object name will be prepended to the message like this: "(cArray)arr".
     */
    cRuntimeError(const cObject *where, const char *msg,...);

    /**
     * We unfortunately need to copy exception objects when handing them back
     * from an activity().
     */
    cRuntimeError(const cRuntimeError& e) : cException(e) {}

    /**
     * Virtual copy constructor. We unfortunately need to copy exception objects
     * when handing them back from an activity().
     */
    virtual cRuntimeError *dup() const {return new cRuntimeError(*this);}
};

/**
 * This exception is only thrown from cSimpleModule::deleteModule()
 * if the current module is to be deleted, in order to exit that module
 * immediately.
 *
 * @ingroup Internals
 */
class SIM_API cDeleteModuleException : public cException
{
  public:
    /**
     * Default ctor.
     */
    cDeleteModuleException() : cException() {}

    /**
     * We unfortunately need to copy exception objects when handing them back
     * from an activity().
     */
    cDeleteModuleException(const cDeleteModuleException& e) : cException(e) {}

    /**
     * Virtual copy constructor. We unfortunately need to copy exception objects
     * when handing them back from an activity().
     */
    virtual cDeleteModuleException *dup() const {return new cDeleteModuleException(*this);}

    /**
     * This exception type does not represent an error condition.
     */
    virtual bool isError() const {return false;}
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
     * Default ctor.
     */
    cStackCleanupException() : cException() {}

    /**
     * We unfortunately need to copy exception objects when handing them back
     * from an activity().
     */
    cStackCleanupException(const cStackCleanupException& e) : cException(e) {}

    /**
     * Virtual copy constructor. We unfortunately need to copy exception objects
     * when handing them back from an activity().
     */
    virtual cStackCleanupException *dup() const {return new cStackCleanupException(*this);}

    /**
     * This exception type does not represent an error condition.
     */
    virtual bool isError() const {return false;}
};

NAMESPACE_END


#endif


