//==========================================================================
//   CLOG.H  -  header for
//                     OMNeT++/OMNEST
//            Discrete System Simulation in C++
//
//==========================================================================

/*--------------------------------------------------------------*
  Copyright (C) 1992-2008 Andras Varga
  Copyright (C) 2006-2008 OpenSim Ltd.

  This file is distributed WITHOUT ANY WARRANTY. See the file
  `license' for details on this and other legal matters.
*--------------------------------------------------------------*/

#ifndef __CLOG_H
#define __CLOG_H

#include <sstream>
#include "simkerneldefs.h"

class cObject;
class cComponent;

/**
 * This class serves as a namespace for log levels and related functions. The actual
 * log levels are not instances of this class, but simply integers from the enumeration
 * declared inside.
 */
class SIM_API cLogLevel
{
  public:
    /**
     * This enumeration is used to classify log messages based on the provided
     * detail and importance. NOTE: log messages are also categorized by the
     * component that generated them.
     */
    enum LogLevel
    {
        /**
         * The highest log level should be used for fatal (unrecoverable) errors
         * that prevent the component from further operation. It doesn't necessarily
         * mean that the simulation must stop immediately and exit. It may simply
         * mean that a component is unable to continue normal operation. For example,
         * a special purpose recording component may be unable to continue recording
         * due to disk full.
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
         * a MAC layer protocol component could log detected collisions and retries
         * using this level.
         */
        LOGLEVEL_WARN,

        /**
         * This log level should be used for high level information that is most
         * likely important for users of the component. For example, a MAC layer
         * protocol component could log successful packet receptions and successful
         * packet transmissions using this level.
         */
        LOGLEVEL_INFO,

        /**
         * This log level should be used for implementation specific details that
         * might be useful and understandable by users of the component. These
         * messages might help to debug various issues without actually looking deep
         * into the code. For example, a MAC layer protocol component could log state
         * machine updates, acknowledges timeouts and selected backoff periods using
         * this level.
         */
        LOGLEVEL_DEBUG,

        /**
         * The lowest log level should be used for very low level implementation specific
         * technical detail that is mostly useful for the actual developers of the
         * component. For example, a MAC layer protocol component could log internal
         * state variable updates, entering or leaving methods using this level.
         */
        LOGLEVEL_TRACE
    };

    /**
     * This log level specifies a globally applied runtime modifiable filter. This is
     * the fastest runtime filter, it works with a simple integer comparison at the call
     * site.
     */
    static LogLevel globalRuntimeLoglevel;

    /**
     * Returns a human readable string representing the provided log level.
     */
    static const char *getName(LogLevel loglevel);

    /**
     * Returns the associated log level for the provided human readable string.
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
#define GLOBAL_COMPILETIME_LOGLEVEL cLogLevel::LOGLEVEL_INFO
#else
#define GLOBAL_COMPILETIME_LOGLEVEL cLogLevel::LOGLEVEL_TRACE
#endif
#endif

/**
 * This macro determines the name of the class where the log statement appears.
 */
#define __CLASS__ typeid(this).name()

/**
 * It returns a log proxy object that captures the provided context.
 * This macro is internal to the logging infrastructure.
 *
 * NOTE: the (void)0 trick prevents GCC producing statement has no effect warnings
 * for compile time disabled log statements.
 */
#define EV_LOGPROXY(object, class, loglevel, category) \
        ((void)0, !(cLogLevel::loglevel <= GLOBAL_COMPILETIME_LOGLEVEL && \
         !ev.isDisabled() && \
         cLogLevel::loglevel <= cLogLevel::globalRuntimeLoglevel && \
         cLogProxy::isEnabled(object, category, cLogLevel::loglevel))) ? \
         cLogProxy::dummyStream : cLogProxy(object, category, cLogLevel::loglevel, __FILE__, __LINE__, class, __FUNCTION__)

/**
 * This macro is similar to a stream and it supposed to be combined with operator<< calls.
 * It returns a log stream associated with the provided log level and log category.
 * Log statements are wrapped with compile time and runtime guards at the call site to
 * efficiently prevent unnecessary computation of parameters and log content.
 */
//@{
#define OPP_LOG(loglevel, category) EV_LOGPROXY(this, __CLASS__, loglevel, category).getStream()
#define OPP_LOG_S(loglevel, category) EV_LOGPROXY((void*)NULL, NULL, loglevel, category).getStream()
//@}

/**
 * This macro is similar to printf and it supposed to be called with a format string and arguments.
 * It returns a log stream associated with the provided log level and category.
 * Log statements are wrapped with compile time and runtime guards at the call site to
 * efficiently prevent unnecessary computation of parameters and log content.
 */
//@{
#define OPP_LOG_P(loglevel, category, format, args...) EV_LOGPROXY(this, __CLASS__, loglevel, category).printf(format, ## args)
#define OPP_LOG_PS(loglevel, category, format, args...) EV_LOGPROXY((void*)NULL, NULL, loglevel, category).printf(format, ## args)
//@}

/**
 * Stream like log macros for class member functions with default category.
 */
//@{
#define EV_FATAL OPP_LOG(LOGLEVEL_FATAL, NULL)
#define EV_ERROR OPP_LOG(LOGLEVEL_ERROR, NULL)
#define EV_WARN  OPP_LOG(LOGLEVEL_WARN, NULL)
#define EV_INFO  OPP_LOG(LOGLEVEL_INFO, NULL)
#define EV_DEBUG OPP_LOG(LOGLEVEL_DEBUG, NULL)
#define EV_TRACE OPP_LOG(LOGLEVEL_TRACE, NULL)
//@}

/**
 * Stream like log macros for static class member or non-member functions with default category.
 */
//@{
#define EV_FATAL_S OPP_LOG_S(LOGLEVEL_FATAL, NULL)
#define EV_ERROR_S OPP_LOG_S(LOGLEVEL_ERROR, NULL)
#define EV_WARN_S  OPP_LOG_S(LOGLEVEL_WARN, NULL)
#define EV_INFO_S  OPP_LOG_S(LOGLEVEL_INFO, NULL)
#define EV_DEBUG_S OPP_LOG_S(LOGLEVEL_DEBUG, NULL)
#define EV_TRACE_S OPP_LOG_S(LOGLEVEL_TRACE, NULL)
//@}

/**
 * Stream like log macros for class member functions with explicit category.
 */
//@{
#define EV_FATAL_C(category) OPP_LOG(LOGLEVEL_FATAL, category)
#define EV_ERROR_C(category) OPP_LOG(LOGLEVEL_ERROR, category)
#define EV_WARN_C(category)  OPP_LOG(LOGLEVEL_WARN, category)
#define EV_INFO_C(category)  OPP_LOG(LOGLEVEL_INFO, category)
#define EV_DEBUG_C(category) OPP_LOG(LOGLEVEL_DEBUG, category)
#define EV_TRACE_C(category) OPP_LOG(LOGLEVEL_TRACE, category)
//@}

/**
 * Stream like log macros for static class member or non-member functions with explicit category.
 */
//@{
#define EV_FATAL_SC(category) OPP_LOG_S(LOGLEVEL_FATAL, category)
#define EV_ERROR_SC(category) OPP_LOG_S(LOGLEVEL_ERROR, category)
#define EV_WARN_SC(category)  OPP_LOG_S(LOGLEVEL_WARN, category)
#define EV_INFO_SC(category)  OPP_LOG_S(LOGLEVEL_INFO, category)
#define EV_DEBUG_SC(category) OPP_LOG_S(LOGLEVEL_DEBUG, category)
#define EV_TRACE_SC(category) OPP_LOG_S(LOGLEVEL_TRACE, category)
//@}

/**
 * Printf like log macros for class member functions with default category.
 */
//@{
#define EV_FATAL_P(format, args...) OPP_LOG_P(LOGLEVEL_FATAL, NULL, format, ## args)
#define EV_ERROR_P(format, args...) OPP_LOG_P(LOGLEVEL_ERROR, NULL, format, ## args)
#define EV_WARN_P(format, args...)  OPP_LOG_P(LOGLEVEL_WARN, NULL, format, ## args)
#define EV_INFO_P(format, args...)  OPP_LOG_P(LOGLEVEL_INFO, NULL, format, ## args)
#define EV_DEBUG_P(format, args...) OPP_LOG_P(LOGLEVEL_DEBUG, NULL, format, ## args)
#define EV_TRACE_P(format, args...) OPP_LOG_P(LOGLEVEL_TRACE, NULL, format, ## args)
//@}

/**
 * Printf like log macros for static class member or non-member functions with default category.
 */
//@{
#define EV_FATAL_PS(format, args...) OPP_LOG_PS(LOGLEVEL_FATAL, NULL, format, ## args)
#define EV_ERROR_PS(format, args...) OPP_LOG_PS(LOGLEVEL_ERROR, NULL, format, ## args)
#define EV_WARN_PS(format, args...)  OPP_LOG_PS(LOGLEVEL_WARN, NULL, format, ## args)
#define EV_INFO_PS(format, args...)  OPP_LOG_PS(LOGLEVEL_INFO, NULL, format, ## args)
#define EV_DEBUG_PS(format, args...) OPP_LOG_PS(LOGLEVEL_DEBUG, NULL, format, ## args)
#define EV_TRACE_PS(format, args...) OPP_LOG_PS(LOGLEVEL_TRACE, NULL, format, ## args)
//@}

/**
 * Printf like log macros for class member functions with explicit category.
 */
//@{
#define EV_FATAL_PC(category, format, args...) OPP_LOG_P(LOGLEVEL_FATAL, category, format, ## args)
#define EV_ERROR_PC(category, format, args...) OPP_LOG_P(LOGLEVEL_ERROR, category, format, ## args)
#define EV_WARN_PC(category, format, args...)  OPP_LOG_P(LOGLEVEL_WARN, category, format, ## args)
#define EV_INFO_PC(category, format, args...)  OPP_LOG_P(LOGLEVEL_INFO, category, format, ## args)
#define EV_DEBUG_PC(category, format, args...) OPP_LOG_P(LOGLEVEL_DEBUG, category, format, ## args)
#define EV_TRACE_PC(category, format, args...) OPP_LOG_P(LOGLEVEL_TRACE, category, format, ## args)
//@}

/**
 * Printf like log macros for static class member or non-member functions with explicit category.
 */
//@{
#define EV_FATAL_PSC(category, format, args...) OPP_LOG_PS(LOGLEVEL_FATAL, category, format, ## args)
#define EV_ERROR_PSC(category, format, args...) OPP_LOG_PS(LOGLEVEL_ERROR, category, format, ## args)
#define EV_WARN_PSC(category, format, args...)  OPP_LOG_PS(LOGLEVEL_WARN, category, format, ## args)
#define EV_INFO_PSC(category, format, args...)  OPP_LOG_PS(LOGLEVEL_INFO, category, format, ## args)
#define EV_DEBUG_PSC(category, format, args...) OPP_LOG_PS(LOGLEVEL_DEBUG, category, format, ## args)
#define EV_TRACE_PSC(category, format, args...) OPP_LOG_PS(LOGLEVEL_TRACE, category, format, ## args)
//@}

/**
 * Support for backward compatibility.
 */
//@{
#define EV   EV_INFO
#define EV_S EV_INFO_S
//@}

/**
 * This class holds various data that is captured when a particular log statement
 * executes. It also contains the text written to the log stream.
 */
class SIM_API cLogEntry
{
  public:
    // log statement related
    cLogLevel::LogLevel loglevel;
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
 * This class is used for buffering the text content to be able to send whole
 * lines one by one to the active environment.
 */
class cLogBuffer: public std::basic_stringbuf<char> {
  public:
    cLogBuffer() { }

    bool isEmpty() { return pptr() == pbase(); }

  protected:
    virtual int sync();
};

/**
 * This class is used for collecting the text content written by multiple
 * invocations of operator<< in a log statement.
 */
class SIM_API cLogStream : public std::ostream
{
  public:
    /**
     * This singleton is used to avoid allocating a new stream each time a
     * log statement executes. It may be also used to write text directly
     * to the log stream.
     */
    static cLogStream globalStream;

  private:
    /**
     * The underlying buffer that contains the text that has been written so far.
     */
    cLogBuffer buffer;

  public:
    cLogStream() : std::ostream(&buffer) { }

    bool isEmpty() { return buffer.isEmpty(); }
    void flushLastLine();
};

/**
 * This class captures the context where the log statement appears.
 *
 * NOTE: This class is internal to the logging infrastructure.
 */
class cLogProxy
{
  friend cLogBuffer;

  public:
    /**
     * This variable is not used at all, it's just used to satisfy the C++ compiler.
     */
    static std::stringstream dummyStream;

  private:
    /**
     * The context of the current (last) log statement that has been executed.
     */
    static cLogEntry currentEntry;

    /**
     * The log level of the previous log statement.
     */
    static cLogLevel::LogLevel previousLoglevel;

    /**
     * The category of the previous log statement.
     */
    static const char *previousCategory;

  public:
    cLogProxy(const void *sourcePointer, const char *category, cLogLevel::LogLevel loglevel, const char *sourceFile, int sourceLine, const char *sourceClass, const char *sourceFunction);
    cLogProxy(const cObject *sourceObject, const char *category, cLogLevel::LogLevel loglevel, const char *sourceFile, int sourceLine, const char *sourceClass, const char *sourceFunction);
    cLogProxy(const cComponent *sourceComponent, const char *category, cLogLevel::LogLevel loglevel, const char *sourceFile, int sourceLine, const char *sourceClass, const char *sourceFunction);
    ~cLogProxy();

    static bool isEnabled(const void *sourceObject, const char *category, cLogLevel::LogLevel loglevel);
    static bool isEnabled(const cComponent *sourceComponent, const char *category, cLogLevel::LogLevel loglevel);
    static bool isComponentEnabled(const cComponent *component, const char *category, cLogLevel::LogLevel loglevel);

    std::ostream& getStream();
    std::ostream& printf(const char *format, ...);

  protected:
    void fillEntry(const char *category, cLogLevel::LogLevel loglevel, const char *sourceFile, int sourceLine, const char *sourceClass, const char *sourceFunction);
};

#endif
