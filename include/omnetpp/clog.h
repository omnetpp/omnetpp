//==========================================================================
//   CLOG.H  -  header for
//                     OMNeT++/OMNEST
//            Discrete System Simulation in C++
//
//==========================================================================

/*--------------------------------------------------------------*
  Copyright (C) 1992-2015 Andras Varga
  Copyright (C) 2006-2015 OpenSim Ltd.

  This file is distributed WITHOUT ANY WARRANTY. See the file
  `license' for details on this and other legal matters.
*--------------------------------------------------------------*/

#ifndef __OMNETPP_CLOG_H
#define __OMNETPP_CLOG_H

#include <ctime>
#include <sstream>
#include "simkerneldefs.h"

NAMESPACE_BEGIN

class cObject;
class cComponent;


/**
 * Classifies log messages based on detail and importance.
 *
 * @ingroup Logging
 */
enum LogLevel
{
    /**
     * The highest log level; it should be used for fatal (unrecoverable) errors
     * that prevent the component from further operation. It doesn't mean that
     * the simulation must stop immediately (because in such cases the code should
     * throw a cRuntimeError), but rather that the a component is unable to continue
     * normal operation. For example, a special purpose recording component may be
     * unable to continue recording due to the disk being full.
     */
    LOGLEVEL_FATAL,

    /**
     * This log level should be used for recoverable (non-fatal) errors that allow
     * the component to continue normal operation. For example, a MAC layer protocol
     * component could log unsuccessful packet receptions and unsuccessful packet
     * transmissions using this level.
     */
    LOGLEVEL_ERROR,

    /**
     * This log level should be used for exceptional (non-error) situations that
     * may be important for users and rarely occur in the component. For example,
     * a MAC layer protocol component could log detected bit errors using this level.
     */
    LOGLEVEL_WARN,

    /**
     * This log level should be used for high-level protocol specific details that
     * are most likely important for the users of the component. For example, a MAC
     * layer protocol component could log successful packet receptions and successful
     * packet transmissions using this level.
     */
    LOGLEVEL_INFO,

    /**
     * This log level should be used for low-level protocol-specific details that
     * may be useful and understandable by the users of the component. These messages
     * may help to track down various protocol-specific issues without actually looking
     * too deep into the code. For example, a MAC layer protocol component could log
     * state machine updates, acknowledge timeouts and selected back-off periods using
     * this level.
     */
    LOGLEVEL_DETAIL,

    /**
     * This log level should be used for high-level implementation-specific technical
     * details that are most likely important for the developers/maintainers of the
     * component. These messages may help to debug various issues when one is looking
     * at the code. For example, a MAC layer protocol component could log updates to
     * internal state variables, updates to complex data structures using this log level.
     */
    LOGLEVEL_DEBUG,

    /**
     * The lowest log level; it should be used for low-level implementation-specific
     * technical details that are mostly useful for the developers/maintainers of the
     * component. For example, a MAC layer protocol component could log control flow
     * in loops and if statements, entering/leaving methods and code blocks using this
     * log level.
     */
    LOGLEVEL_TRACE
};


/**
 * Serves as a namespace for log levels related functions.
 * @see LogLevel
 *
 * @ingroup Logging
 */
class SIM_API cLogLevel
{
  public:
    /**
     * This log level specifies a globally applied runtime modifiable filter. This is
     * the fastest runtime filter, it works with a simple integer comparison at the call
     * site.
     */
    static LogLevel globalRuntimeLoglevel;

    /**
     * Returns a human-readable string representing the provided log level.
     */
    static const char *getName(LogLevel loglevel);

    /**
     * Returns the associated log level for the provided human-readable string.
     */
    static LogLevel getLevel(const char *name);
};

/**
 * This log level specifies a globally applied compile time filter. Log statements
 * that use lower log levels than the one specified by this macro are not compiled
 * into the executable.
 */
#ifndef GLOBAL_COMPILETIME_LOGLEVEL
#ifdef NDEBUG
#define GLOBAL_COMPILETIME_LOGLEVEL LOGLEVEL_INFO
#else
#define GLOBAL_COMPILETIME_LOGLEVEL LOGLEVEL_TRACE
#endif
#endif


// Creates a log proxy object that captures the provided context.
// This macro is internal to the logging infrastructure.
//
// NOTE: the (void)0 trick prevents GCC producing statement has no effect warnings
// for compile time disabled log statements.
//
#define OPP_LOGPROXY(object, classname, loglevel, category) \
        ((void)0, !(loglevel <= GLOBAL_COMPILETIME_LOGLEVEL && \
         !getEnvir()->isDisabled() && \
         loglevel <= OPP::cLogLevel::globalRuntimeLoglevel && \
         OPP::cLogProxy::isEnabled(object, category, loglevel))) ? \
         OPP::cLogProxy::dummyStream : OPP::cLogProxy(object, category, loglevel, __FILE__, __LINE__, classname, __FUNCTION__)


// Returns NULL. Helper function for the logging macros.
inline void *getThisPtr() {return NULL;}

// Returns NULL. Helper function for the logging macros.
inline const char *getClassName() {return NULL;}

/**
 * Use this macro when logging from static member functions.
 * Background: OPP_LOG and derived macros (EV_INFO, EV_DETAIL, etc) will fail
 * to compile when placed into static member functions of cObject-derived classes
 * ("cannot call member function 'cObject::getThisPtr()' without object" in GNU C++,
 * and "C2352: illegal call of non-static member function" in Visual C++).
 * To fix it, add this macro at the top of the function; it contains local declarations
 * to make the code compile.
 *
 * @ingroup Logging
 * @hideinitializer
 */
#define EV_STATICCONTEXT  void *(*getThisPtr)() = OPP::getThisPtr; const char *(*getClassName)() = OPP::getClassName;

/**
 * This is the macro underlying EV_INFO, EV_DETAIL, EV_INFO_C, and similar log macros.
 * It can be used as a C++ stream to log data with the provided log level and log category.
 * Log statements are wrapped with compile time and runtime guards at the call site to
 * efficiently prevent unnecessary computation of parameters and log content.
 *
 * @see LogLevel, EV_STATICCONTEXT, EV_INFO, EV_INFO_C
 * @ingroup Logging
 * @hideinitializer
 */
#define OPP_LOG(loglevel, category) OPP_LOGPROXY(getThisPtr(), getClassName(), loglevel, category).getStream()

/**
 * Logging macros with the default category. Category is a string that refers to the
 * topic of the log message. The macro can be used as a C++ stream. Example:
 * <tt>EV_DETAIL << "Connection setup complete" << endl;</tt>.
 *
 * @see OPP_LOG, LogLevel, EV_STATICCONTEXT
 * @ingroup Logging
 * @defgroup LoggingDefault Logging with default category
 */
//@{
#define EV        EV_INFO                        ///< Short for EV_INFO
#define EV_FATAL  OPP_LOG(LOGLEVEL_FATAL, NULL)  ///< Log local fatal errors
#define EV_ERROR  OPP_LOG(LOGLEVEL_ERROR, NULL)  ///< Log local but recoverable errors
#define EV_WARN   OPP_LOG(LOGLEVEL_WARN, NULL)   ///< Log warnings
#define EV_INFO   OPP_LOG(LOGLEVEL_INFO, NULL)   ///< Log information (default log level)
#define EV_DETAIL OPP_LOG(LOGLEVEL_DETAIL, NULL) ///< Log state variables and other low-level information
#define EV_DEBUG  OPP_LOG(LOGLEVEL_DEBUG, NULL)  ///< Log state variables and other low-level information
#define EV_TRACE  OPP_LOG(LOGLEVEL_TRACE, NULL)  ///< Log control flow information (entering/exiting functions, etc)
//@}

/**
 * Logging macros with an explicit category. Category is a string that refers to the
 * topic of the log message. The macro can be used as a C++ stream. Example:
 * <tt>EV_DETAIL("retransmissions") << "Too many retries, discarding frame" << endl;</tt>.
 *
 * @see OPP_LOG, LogLevel, EV_STATICCONTEXT
 * @ingroup Logging
 * @defgroup LoggingCat  Logging with explicit category
 */
//@{
#define EV_C(category)        EV_INFO_C(category)                ///< Short for EV_INFO_C
#define EV_FATAL_C(category)  OPP_LOG(LOGLEVEL_FATAL, category)  ///< Log local fatal errors
#define EV_ERROR_C(category)  OPP_LOG(LOGLEVEL_ERROR, category)  ///< Log local but recoverable errors
#define EV_WARN_C(category)   OPP_LOG(LOGLEVEL_WARN, category)   ///< Log warnings
#define EV_INFO_C(category)   OPP_LOG(LOGLEVEL_INFO, category)   ///< Log information (default log level)
#define EV_DETAIL_C(category) OPP_LOG(LOGLEVEL_DETAIL, category) ///< Log state variables and other low-level information
#define EV_DEBUG_C(category)  OPP_LOG(LOGLEVEL_DEBUG, category)  ///< Log state variables and other low-level information
#define EV_TRACE_C(category)  OPP_LOG(LOGLEVEL_TRACE, category)  ///< Log control flow information (entering/exiting functions, etc)
//@}

/**
 * This class holds various data that is captured when a particular log statement
 * executes. It also contains the text written to the log stream.
 * @ingroup Internals
 */
class SIM_API cLogEntry
{
  public:
    // log statement related
    LogLevel loglevel;
    const char *category;

    // C++ source related (where the log statement appears)
    const void *sourcePointer;
    const cObject *sourceObject;
    const cComponent *sourceComponent;
    const char *sourceFile;
    int sourceLine;
    const char *sourceClass;
    const char *sourceFunction;

    // operating system related
    clock_t userTime;

    // the actual text of the log statement
    const char *text;
    int textLength;
};


/**
 * This class captures the context where the log statement appears.
 * NOTE: This class is internal to the logging infrastructure.
 *
 * @ingroup Internals
 */
class SIM_API cLogProxy
{
  private:
    /**
     * This class is used for buffering the text content to be able to send whole
     * lines one by one to the active environment.
     */
    class LogBuffer : public std::basic_stringbuf<char> {
      public:
        LogBuffer() { }
        bool isEmpty() { return pptr() == pbase(); }
      protected:
        virtual int sync();  // invokes getEnvir()->log() for each log line
    };

    // act likes /dev/null
    class nullstream : public std::ostream {
      public:
        nullstream() : std::ostream(0) {}  // results in rdbuf==0 and badbit==true
    };

  public:
    static nullstream dummyStream; // EV evaluates to this when in express mode (getEnvir()->disabled())

  private:
    static LogBuffer buffer;  // underlying buffer that contains the text that has been written so far
    static std::ostream globalStream;  // this singleton is used to avoid allocating a new stream each time a log statement executes
    static cLogEntry currentEntry; // context of the current (last) log statement that has been executed.
    static LogLevel previousLoglevel; // log level of the previous log statement
    static const char *previousCategory; // category of the previous log statement

  public:
    cLogProxy(const void *sourcePointer, const char *category, LogLevel loglevel, const char *sourceFile, int sourceLine, const char *sourceClass, const char *sourceFunction);
    cLogProxy(const cObject *sourceObject, const char *category, LogLevel loglevel, const char *sourceFile, int sourceLine, const char *sourceClass, const char *sourceFunction);
    cLogProxy(const cComponent *sourceComponent, const char *category, LogLevel loglevel, const char *sourceFile, int sourceLine, const char *sourceClass, const char *sourceFunction);
    ~cLogProxy();

    static bool isEnabled(const void *sourceObject, const char *category, LogLevel loglevel);
    static bool isEnabled(const cComponent *sourceComponent, const char *category, LogLevel loglevel);
    static bool isComponentEnabled(const cComponent *component, const char *category, LogLevel loglevel);

    std::ostream& getStream() { return globalStream; }
    static void flushLastLine();

  protected:
    void fillEntry(const char *category, LogLevel loglevel, const char *sourceFile, int sourceLine, const char *sourceClass, const char *sourceFunction);
};

NAMESPACE_END

#endif
