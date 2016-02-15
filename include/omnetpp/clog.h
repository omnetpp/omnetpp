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

namespace omnetpp {

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
     * The lowest log level; it should be used for low-level implementation-specific
     * technical details that are mostly useful for the developers/maintainers of the
     * component. For example, a MAC layer protocol component could log control flow
     * in loops and if statements, entering/leaving methods and code blocks using this
     * log level.
     */
    LOGLEVEL_TRACE,

    /**
     * This log level should be used for high-level implementation-specific technical
     * details that are most likely important for the developers/maintainers of the
     * component. These messages may help to debug various issues when one is looking
     * at the code. For example, a MAC layer protocol component could log updates to
     * internal state variables, updates to complex data structures using this log level.
     */
    LOGLEVEL_DEBUG,

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
     * This log level should be used for high-level protocol specific details that
     * are most likely important for the users of the component. For example, a MAC
     * layer protocol component could log successful packet receptions and successful
     * packet transmissions using this level.
     */
    LOGLEVEL_INFO,

    /**
     * This log level should be used for exceptional (non-error) situations that
     * may be important for users and rarely occur in the component. For example,
     * a MAC layer protocol component could log detected bit errors using this level.
     */
    LOGLEVEL_WARN,

    /**
     * This log level should be used for recoverable (non-fatal) errors that allow
     * the component to continue normal operation. For example, a MAC layer protocol
     * component could log unsuccessful packet receptions and unsuccessful packet
     * transmissions using this level.
     */
    LOGLEVEL_ERROR,

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
     * Not a real log level. TODO
     */
    LOGLEVEL_OFF,
};

/**
 * This log level specifies a globally applied compile time filter. Log statements
 * that use lower log levels than the one specified by this macro are not compiled
 * into the executable.
 */
#ifndef COMPILETIME_LOGLEVEL
#ifdef NDEBUG
#define COMPILETIME_LOGLEVEL LOGLEVEL_INFO
#else
#define COMPILETIME_LOGLEVEL LOGLEVEL_TRACE
#endif
#endif

/**
 * This predicate determines if a log statement gets compiled into the executable.
 */
#ifndef COMPILETIME_LOG_PREDICATE
#define COMPILETIME_LOG_PREDICATE(object, logLevel, category) (logLevel >= COMPILETIME_LOGLEVEL)
#endif

/**
 * This class groups logging related functionality.
 * @see LogLevel
 *
 * @ingroup Logging
 */
class SIM_API cLog
{
  public:
    typedef bool (*NoncomponentLogPredicate)(const void *object, LogLevel logLevel, const char *category);
    typedef bool (*ComponentLogPredicate)(const cComponent *object, LogLevel logLevel, const char *category);

  public:
    /**
     * This log level specifies a globally applied runtime modifiable filter. This is
     * the fastest runtime filter, it works with a simple integer comparison at the call
     * site.
     */
    static LogLevel logLevel;

    static NoncomponentLogPredicate noncomponentLogPredicate;
    static ComponentLogPredicate componentLogPredicate;

  public:
    /**
     * Returns a human-readable string representing the provided log level.
     */
    static const char *getLogLevelName(LogLevel logLevel);

    /**
     * Returns the associated log level for the provided human-readable string.
     */
    static LogLevel resolveLogLevel(const char *name);

    static inline bool runtimeLogPredicate(const void *object, LogLevel logLevel, const char *category)
    { return noncomponentLogPredicate(object, logLevel, category); }

    static inline bool runtimeLogPredicate(const cComponent *object, LogLevel logLevel, const char *category)
    { return componentLogPredicate(object, logLevel, category); }

    static bool defaultNoncomponentLogPredicate(const void *object, LogLevel logLevel, const char *category);
    static bool defaultComponentLogPredicate(const cComponent *object, LogLevel logLevel, const char *category);
};

// Creates a log proxy object that captures the provided context.
// This macro is internal to the logging infrastructure.
//
// NOTE: the (void)0 trick prevents GCC producing statement has no effect warnings
// for compile time disabled log statements.
//
#define OPP_LOGPROXY(object, logLevel, category) \
    ((void)0, !(COMPILETIME_LOG_PREDICATE(object, logLevel, category) && \
    cLog::runtimeLogPredicate(object, logLevel, category))) ? \
    omnetpp::cLogProxy::dummyStream : omnetpp::cLogProxy(object, logLevel, category, __FILE__, __LINE__, getClassName(), __FUNCTION__)


// Returns nullptr. Helper function for the logging macros.
inline void *getThisPtr() {return nullptr;}

// Returns nullptr. Helper function for the logging macros.
inline const char *getClassName() {return nullptr;}

/**
 * Use this macro when logging from static member functions.
 * Background: EV_LOG and derived macros (EV_INFO, EV_DETAIL, etc) will fail
 * to compile when placed into static member functions of cObject-derived classes
 * ("cannot call member function 'cObject::getThisPtr()' without object" in GNU C++,
 * and "C2352: illegal call of non-static member function" in Visual C++).
 * To fix it, add this macro at the top of the function; it contains local declarations
 * to make the code compile.
 *
 * @ingroup Logging
 * @hideinitializer
 */
#define EV_STATICCONTEXT  void *(*getThisPtr)() = omnetpp::getThisPtr; const char *(*getClassName)() = omnetpp::getClassName;

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
#define EV_LOG(logLevel, category) OPP_LOGPROXY(getThisPtr(), logLevel, category).getStream()

/**
 * Logging macros with the default category. Category is a string that refers to the
 * topic of the log message. The macro can be used as a C++ stream. Example:
 * <tt>EV_DETAIL << "Connection setup complete" << endl;</tt>.
 *
 * @see EV_LOG, LogLevel, EV_STATICCONTEXT
 * @ingroup Logging
 * @defgroup LoggingDefault Logging with default category
 */
//@{
#define EV        EV_INFO                           ///< Short for EV_INFO
#define EV_FATAL  EV_LOG(LOGLEVEL_FATAL, nullptr)  ///< Log local fatal errors
#define EV_ERROR  EV_LOG(LOGLEVEL_ERROR, nullptr)  ///< Log local but recoverable errors
#define EV_WARN   EV_LOG(LOGLEVEL_WARN, nullptr)   ///< Log warnings
#define EV_INFO   EV_LOG(LOGLEVEL_INFO, nullptr)   ///< Log information (default log level)
#define EV_DETAIL EV_LOG(LOGLEVEL_DETAIL, nullptr) ///< Log state variables and other low-level information
#define EV_DEBUG  EV_LOG(LOGLEVEL_DEBUG, nullptr)  ///< Log state variables and other low-level information
#define EV_TRACE  EV_LOG(LOGLEVEL_TRACE, nullptr)  ///< Log control flow information (entering/exiting functions, etc)
//@}

/**
 * Logging macros with an explicit category. Category is a string that refers to the
 * topic of the log message. The macro can be used as a C++ stream. Example:
 * <tt>EV_DETAIL("retransmissions") << "Too many retries, discarding frame" << endl;</tt>.
 *
 * @see EV_LOG, LogLevel, EV_STATICCONTEXT
 * @ingroup Logging
 * @defgroup LoggingCat  Logging with explicit category
 */
//@{
#define EV_C(category)        EV_INFO_C(category)                ///< Short for EV_INFO_C
#define EV_FATAL_C(category)  EV_LOG(LOGLEVEL_FATAL, category)  ///< Log local fatal errors
#define EV_ERROR_C(category)  EV_LOG(LOGLEVEL_ERROR, category)  ///< Log local but recoverable errors
#define EV_WARN_C(category)   EV_LOG(LOGLEVEL_WARN, category)   ///< Log warnings
#define EV_INFO_C(category)   EV_LOG(LOGLEVEL_INFO, category)   ///< Log information (default log level)
#define EV_DETAIL_C(category) EV_LOG(LOGLEVEL_DETAIL, category) ///< Log state variables and other low-level information
#define EV_DEBUG_C(category)  EV_LOG(LOGLEVEL_DEBUG, category)  ///< Log state variables and other low-level information
#define EV_TRACE_C(category)  EV_LOG(LOGLEVEL_TRACE, category)  ///< Log control flow information (entering/exiting functions, etc)
//@}

/**
 * This class holds various data that is captured when a particular log statement
 * executes. It also contains the text written to the log stream.
 *
 * @ingroup Internals
 */
class SIM_API cLogEntry
{
  public:
    // log statement related
    LogLevel logLevel;
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


//
// This class captures the context where the log statement appears.
// NOTE: This class is internal to the logging infrastructure.
//
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
        virtual int sync() override;  // invokes getEnvir()->log() for each log line
    };

    // act likes /dev/null
    class nullstream : public std::ostream {
      public:
        nullstream() : std::ostream(nullptr) {}  // results in rdbuf==0 and badbit==true
    };

  public:
    static nullstream dummyStream; // EV evaluates to this when in express mode (getEnvir()->disabled())

  private:
    static LogBuffer buffer;  // underlying buffer that contains the text that has been written so far
    static std::ostream stream;  // this singleton is used to avoid allocating a new stream each time a log statement executes
    static cLogEntry currentEntry; // context of the current (last) log statement that has been executed.
    static LogLevel previousLogLevel; // log level of the previous log statement
    static const char *previousCategory; // category of the previous log statement

  private:
    void fillEntry(LogLevel logLevel, const char *category, const char *sourceFile, int sourceLine, const char *sourceClass, const char *sourceFunction);

  public:
    cLogProxy(const void *sourcePointer, LogLevel logLevel, const char *category, const char *sourceFile, int sourceLine, const char *sourceClass, const char *sourceFunction);
    cLogProxy(const cObject *sourceObject, LogLevel logLevel, const char *category, const char *sourceFile, int sourceLine, const char *sourceClass, const char *sourceFunction);
    cLogProxy(const cComponent *sourceComponent, LogLevel logLevel, const char *category, const char *sourceFile, int sourceLine, const char *sourceClass, const char *sourceFunction);
    ~cLogProxy();

    std::ostream& getStream() { return stream; }
    static void flushLastLine();
};

}  // namespace omnetpp

#endif
