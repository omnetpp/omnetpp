//==========================================================================
//   CLOG.H  -  header for
//                     OMNeT++/OMNEST
//            Discrete System Simulation in C++
//
//==========================================================================

/*--------------------------------------------------------------*
  Copyright (C) 1992-2017 Andras Varga
  Copyright (C) 2006-2017 OpenSim Ltd.

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
 * @brief Classifies log messages based on detail and importance.
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
     * Not a real log level, it completely disables logging when set.
     */
    LOGLEVEL_OFF,

    /**
     * Not a real log level, used for indicating the absence of a log level setting.
     */
    LOGLEVEL_NOTSET = -1
};

/**
 * @brief For compile-time filtering of logs.
 *
 * One is free to define this macro before including <omnetpp.h>, or redefine
 * it any time. The change will affect subsequent log statements.
 * Log statements that use lower log levels than the one specified
 * by this macro will not be compiled into the executable.
 *
 * @ingroup Logging
 */
#ifndef COMPILETIME_LOGLEVEL
#ifdef NDEBUG
#define COMPILETIME_LOGLEVEL omnetpp::LOGLEVEL_DETAIL
#else
#define COMPILETIME_LOGLEVEL omnetpp::LOGLEVEL_TRACE
#endif
#endif

constexpr bool defaultCompiletimeLogPredicate(LogLevel logLevel, LogLevel compiletimeLogLevel)
{
    // The COMPILETIME_LOGLEVEL macro is not used directly here,
    // as that would prevent its redefinitions from taking effect.
    // Rather, it is taken as a parameter, so COMPILETIME_LOGLEVEL
    // is substituted at each call site with its current value.
    return logLevel >= compiletimeLogLevel;
}

/**
 * @brief This predicate determines if a log statement gets compiled into the
 * executable.
 *
 * One is free to define this macro before including <omnetpp.h>, or redefine
 * it any time. The change will affect subsequent log statements.
 *
 * @ingroup Logging
 */
#ifndef COMPILETIME_LOG_PREDICATE
#define COMPILETIME_LOG_PREDICATE(object, logLevel, category) \
    (::omnetpp::defaultCompiletimeLogPredicate(logLevel, COMPILETIME_LOGLEVEL))
#endif

/**
 * @brief This class groups logging related functionality.
 *
 * @see LogLevel
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

    /**
     * This predicate determines if a log statement is executed for log statements
     * that occur outside module or channel member functions. This is a customization
     * point for logging.
     */
    static NoncomponentLogPredicate noncomponentLogPredicate;

    /**
     * This predicate determines if a log statement is executed for log statements
     * that occur in module or channel member functions. This is a customization
     * point for logging.
     */
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
    omnetpp::cLog::runtimeLogPredicate(object, logLevel, category))) ? \
    omnetpp::internal::cLogProxy::dummyStream : omnetpp::internal::cLogProxy(object, logLevel, category, __FILE__, __LINE__, __FUNCTION__)

// Returns nullptr. Helper function for the logging macros.
inline void *getThisPtr() {return nullptr;}

/**
 * @brief Use this macro when logging from static member functions.
 *
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
#define EV_STATICCONTEXT  void *(*getThisPtr)() = omnetpp::getThisPtr;

/**
 * @brief This is the macro underlying EV_INFO, EV_DETAIL, EV_INFO_C, and
 * similar log macros.
 *
 * This macro should not be used directly, but via the logging macros
 * EV, EV_FATAL, EV_ERROR, EV_WARN, EV_INFO, EV_DETAIL, EV_DEBUG, EV_TRACE,
 * and their "category" versions EV_C, EV_FATAL_C, EV_ERROR_C, EV_WARN_C,
 * EV_INFO_C, EV_DETAIL_C, EV_DEBUG_C, EV_TRACE_C.
 *
 * Those macros act as C++ streams: one can write on them using the
 * left-shift (<<) operator. Their names refer to the log level they
 * represent (see LogLevel). The "category" (_C) versions accept a category
 * string. Each category acts like a separate log channel; for example,
 * one can use the "test" category to log text intended for consumption
 * by an automated test suite.
 *
 * Log statements are wrapped with compile-time and runtime guards at the
 * call site to efficiently prevent unnecessary computation of parameters
 * and log content. Compile-time guards are COMPILETIME_LOGLEVEL and
 * COMPILETIME_LOG_PREDICATE. Runtime guards (runtime log level) can be
 * set up via omnetpp.ini.
 *
 * Under certain circumstances, compiling log statements may result in errors.
 * When that happens, it is possible that the EV_STATICCONTEXT macro needs to
 * be added to the code; please review its documentation for more info.
 *
 * Examples:
 *
 * \code
 * EV << "Connection setup complete" << endl;  // equivalent to EV_INFO << ...
 * EV_DEBUG << "retryCount=" << retryCount << std::endl;
 * EV_WARNING << "Connection timed out\n";
 * EV_INFO_C("testing") << "ESTAB" << endl;
 * \endcode
 *
 * @see LogLevel, EV_STATICCONTEXT, EV_INFO, EV_INFO_C, COMPILETIME_LOGLEVEL, COMPILETIME_LOG_PREDICATE
 * @ingroup Logging
 * @hideinitializer
 */
#define EV_LOG(logLevel, category) OPP_LOGPROXY(getThisPtr(), logLevel, category).getStream()

/**
 * @brief Short for EV_INFO.
 * @see EV_INFO @ingroup Logging
 */
#define EV        EV_INFO

/**
 * @brief Pseudo-stream for logging local fatal errors. See EV_LOG for details.
 * @see EV_LOG @hideinitializer @ingroup Logging
 */
#define EV_FATAL  EV_LOG(omnetpp::LOGLEVEL_FATAL, nullptr)

/**
 * @brief Pseudo-stream for logging local recoverable errors. See EV_LOG for details.
 * @see EV_LOG @hideinitializer @ingroup Logging
 */
#define EV_ERROR  EV_LOG(omnetpp::LOGLEVEL_ERROR, nullptr)

/**
 * @brief Pseudo-stream for logging warnings. See EV_LOG for details.
 * @see EV_LOG @hideinitializer @ingroup Logging
 */
#define EV_WARN   EV_LOG(omnetpp::LOGLEVEL_WARN, nullptr)

/**
 * @brief Pseudo-stream for logging information with the default log level. See EV_LOG for details.
 * @see EV_LOG @hideinitializer @ingroup Logging
 */
#define EV_INFO   EV_LOG(omnetpp::LOGLEVEL_INFO, nullptr)

/**
 * @brief Pseudo-stream for logging low-level protocol-specific details. See EV_LOG for details.
 * @see EV_LOG @hideinitializer @ingroup Logging
 */
#define EV_DETAIL EV_LOG(omnetpp::LOGLEVEL_DETAIL, nullptr)

/**
 * @brief Pseudo-stream for logging state variables and other low-level information. See EV_LOG for details.
 * @see EV_LOG @hideinitializer @ingroup Logging
 */
#define EV_DEBUG  EV_LOG(omnetpp::LOGLEVEL_DEBUG, nullptr)

/**
 * @brief Pseudo-stream for logging control flow information (entering/exiting functions, etc). See EV_LOG for details.
 * @see EV_LOG @hideinitializer @ingroup Logging
 */
#define EV_TRACE  EV_LOG(omnetpp::LOGLEVEL_TRACE, nullptr)

/**
 * @brief Short for EV_INFO_C.
 * @see EV_INFO_C @ingroup Logging
 */
#define EV_C(category)        EV_INFO_C(category)

/**
 * @brief Pseudo-stream for logging local fatal errors of a specific category. See EV_LOG for details.
 * @see EV_LOG @hideinitializer @ingroup Logging
 */
#define EV_FATAL_C(category)  EV_LOG(omnetpp::LOGLEVEL_FATAL, category)

/**
 * @brief Pseudo-stream for logging local recoverable errors of a specific category. See EV_LOG for details.
 * @see EV_LOG @hideinitializer @ingroup Logging
 */
#define EV_ERROR_C(category)  EV_LOG(omnetpp::LOGLEVEL_ERROR, category)

/**
 * @brief Pseudo-stream for logging warnings of a specific category. See EV_LOG for details.
 * @see EV_LOG @hideinitializer @ingroup Logging
 */
#define EV_WARN_C(category)   EV_LOG(omnetpp::LOGLEVEL_WARN, category)

/**
 * @brief Pseudo-stream for logging information with the default log level of a specific category. See EV_LOG for details.
 * @see EV_LOG @hideinitializer @ingroup Logging
 */
#define EV_INFO_C(category)   EV_LOG(omnetpp::LOGLEVEL_INFO, category)

/**
 * @brief Pseudo-stream for logging low-level protocol-specific details of a specific category. See EV_LOG for details.
 * @see EV_LOG @hideinitializer @ingroup Logging
 */
#define EV_DETAIL_C(category) EV_LOG(omnetpp::LOGLEVEL_DETAIL, category)

/**
 * @brief Pseudo-stream for logging state variables and other low-level information of a specific category. See EV_LOG for details.
 * @see EV_LOG @hideinitializer @ingroup Logging
 */
#define EV_DEBUG_C(category)  EV_LOG(omnetpp::LOGLEVEL_DEBUG, category)

/**
 * @brief Pseudo-stream for logging control flow information (entering/exiting functions, etc) of a specific category. See EV_LOG for details.
 * @see EV_LOG @hideinitializer @ingroup Logging
 */
#define EV_TRACE_C(category)  EV_LOG(omnetpp::LOGLEVEL_TRACE, category)

/**
 * @brief This class holds various data that is captured when a particular
 * log statement executes. It also contains the text written to the log stream.
 *
 * @see cEnvir::log(cLogEntry*)
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
    const char *sourceFunction;

    // operating system related
    clock_t userTime;

    // the actual text of the log statement
    const char *text;
    int textLength;
};

 namespace internal {

//
// This class captures the context where the log statement appears.
// NOTE: This class is internal to the logging infrastructure.
//
class SIM_API cLogProxy
{
  private:
    // This class is used for buffering the text content to be able to send whole
    // lines one by one to the active environment.
    class LogBuffer : public std::basic_stringbuf<char> {
      public:
        LogBuffer() {}
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
    void fillEntry(LogLevel logLevel, const char *category, const char *sourceFile, int sourceLine, const char *sourceFunction);

  public:
    cLogProxy(const void *sourcePointer, LogLevel logLevel, const char *category, const char *sourceFile, int sourceLine, const char *sourceFunction);
    cLogProxy(const cObject *sourceObject, LogLevel logLevel, const char *category, const char *sourceFile, int sourceLine, const char *sourceFunction);
    cLogProxy(const cComponent *sourceComponent, LogLevel logLevel, const char *category, const char *sourceFile, int sourceLine, const char *sourceFunction);
    ~cLogProxy();

    std::ostream& getStream() { return stream; }
    static void flushLastLine();
};

}  // namespace internal

}  // namespace omnetpp

#endif
