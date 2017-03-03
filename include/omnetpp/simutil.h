//==========================================================================
//  SIMUTIL.H - part of
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

#ifndef __OMNETPP_SIMUTIL_H
#define __OMNETPP_SIMUTIL_H

#include <cstring>  // for strlen, etc.
#include <cstdarg>  // for va_list
#include <cstdio>   // for sprintf
#include <cstdlib>  // for gcvt
#include <typeinfo>  // for type_info
#include <string>    // for std::string
#include "platdep/platmisc.h" // for gcvt, etc
#include "simkerneldefs.h"
#include "errmsg.h"

namespace omnetpp {

// forward declarations
class cComponent;

// logically belongs to csimulation.h but must be here because of declaration order
enum {CTX_NONE, CTX_BUILD, CTX_INITIALIZE, CTX_EVENT, CTX_REFRESHDISPLAY, CTX_FINISH, CTX_CLEANUP};

/** @addtogroup Utilities */
//@{
/**
 * @brief Same as the standard strlen() function, except that it also accepts
 * nullptr and returns 0 for it.
 */
inline int opp_strlen(const char *);

/**
 * @brief Duplicates the string, using <tt>new char[]</tt>. For nullptr and empty
 * strings it returns nullptr.
 */
inline char *opp_strdup(const char *);

/**
 * @brief Same as the standard strcpy() function, except that nullptr as the
 * second argument is treated as an empty string ("").
 */
inline char *opp_strcpy(char *,const char *);

/**
 * @brief Same as the standard strcmp() function, except that nullptr is treated
 * exactly as an empty string ("").
 */
inline int opp_strcmp(const char *, const char *);

/**
 * @brief Copies src string into dest, and if its length would exceed maxlen,
 * it is truncated with an ellipsis. For example, <tt>opp_strprettytrunc(buf,
 * "long-long",6)</tt> yields <tt>"lon..."</tt>.
 */
SIM_API char *opp_strprettytrunc(char *dest, const char *src, unsigned maxlen);

/**
 * @brief Returns the name of a C++ type, correcting the quirks of various compilers.
 */
SIM_API const char *opp_typename(const std::type_info& t);

/**
 * @brief Returns a monotonic time in microseconds since some unspecified
 * starting point. This clock is not affected by discontinuous jumps in the
 * system time (e.g. if the system administrator manually changes the clock).
 * Note that the actual resolution (precision) of the clock may be less than
 * microseconds.
 */
SIM_API int64_t opp_get_monotonic_clock_usecs();  // in gettime.cc

//@}

// INTERNAL: returns the name of a C++ type, correcting the quirks of various compilers.
SIM_API const char *opp_demangle_typename(const char *mangledName);


/**
 * @brief Denotes module class member function as callable from other modules.
 *
 * Usage: <tt>Enter_Method(fmt, arg1, arg2...);</tt>
 *
 * Example: <tt>Enter_Method("requestPacket(%d)",n);</tt>
 *
 * The macro should be put at the top of every module member function
 * that may be called from other modules. This macro arranges to
 * temporarily switch the context to the called module (the old context
 * will be restored automatically when the method returns),
 * and also lets the graphical user interface animate the method call.
 *
 * The argument(s) should specify the method name (and parameters) --
 * it will be used for the animation. The argument list works as in
 * <tt>printf()</tt>, so it is easy to include the actual parameter values.
 *
 * @see Enter_Method_Silent() macro
 * @ingroup SimSupport
 * @hideinitializer
 */
#define Enter_Method  omnetpp::cMethodCallContextSwitcher __ctx(this); __ctx.methodCall

/**
 * @brief Denotes module class member function as callable from other modules.
 *
 * This macro is similar to the Enter_Method() macro, only it does not animate
 * the call on the GUI; the call is still recorded into the the event log file.
 *
 * The macro may be called with or without arguments. When called with arguments,
 * they should be a printf-style format string, and parameters to be substituted
 * into it; the resulting string should contain the method name and the actual
 * arguments.
 *
 * Usage: <tt>Enter_Method_Silent();</tt>, <tt>Enter_Method_Silent(fmt, arg1, arg2...);</tt>
 *
 * Example: <tt>Enter_Method_Silent("getRouteFor(address=%d)",address);</tt>
 *
 * @see Enter_Method() macro
 * @ingroup SimSupport
 * @hideinitializer
 */
#define Enter_Method_Silent  omnetpp::cMethodCallContextSwitcher __ctx(this); __ctx.methodCallSilent

/**
 * @brief The constructor switches the context to the given component, and the
 * destructor restores the original context.
 *
 * @see cSimulation::getContextModule(), cSimulation::setContextModule()
 * @ingroup Internals
 */
class SIM_API cContextSwitcher
{
  protected:
    cComponent *callerContext;
  public:
    /**
     * Switches context to the given module
     */
    cContextSwitcher(const cComponent *newContext);

    /**
     * Restores the original context
     */
    ~cContextSwitcher();
};

/**
 * @brief Internal class. May only be used via the Enter_Method() and Enter_Method_Silent() macros!
 * @ingroup Internals
 */
class SIM_API cMethodCallContextSwitcher : public cContextSwitcher
{
  private:
    static int depth;

  public:
    /**
     * Switches context to the given module
     */
    //TODO store previous frame, __FILE__, __LINE__, __FUNCTION__ too, at least in debug builds?
    cMethodCallContextSwitcher(const cComponent *newContext);

    /**
     * Restores the original context
     */
    ~cMethodCallContextSwitcher();

    /**
     * Various ways to tell the user interface about the method call so that
     * the call can be animated, recorded into the event log, etc.
     */
    void methodCall(const char *methodFmt,...);
    void methodCallSilent(const char *methodFm,...);
    void methodCallSilent();

    /**
     * Returns the depth of Enter_Method[_Silent] calls
     */
    static int getDepth() {return depth;}
};

/**
 * @brief The constructor switches the context type, and the destructor restores
 * the original context type.
 *
 * @see cSimulation::getContextModule(), cSimulation::setContextModule()
 * @ingroup Internals
 */
class SIM_API cContextTypeSwitcher
{
  private:
    int savedcontexttype;

  public:
    /**
     * Switches the context type (see CTX_xxx constants)
     */
    cContextTypeSwitcher(int contexttype);

    /**
     * Restores the original context type
     */
    ~cContextTypeSwitcher();
};

// implementations:

inline char *opp_strcpy(char *s1,const char *s2)
{
    return strcpy(s1, s2 ? s2 : "");
}

inline int opp_strlen(const char *s)
{
    return s ? strlen(s) : 0;
}

inline char *opp_strdup(const char *s)
{
    if (!s || !s[0]) return nullptr;
    char *p = new char[strlen(s)+1];
    strcpy(p,s);
    return p;
}

inline char *opp_strdup(const char *s, int len)
{
    if (!s || !s[0]) return nullptr;
    char *p = new char[len+1];
    strncpy(p,s,len);
    p[len] = 0;
    return p;
}

inline int opp_strcmp(const char *s1, const char *s2)
{
    if (s1)
        return s2 ? strcmp(s1,s2) : (*s1 ? 1 : 0);
    else
        return (s2 && *s2) ? -1 : 0;
}

// internally used: appends [num] to the given string
inline void opp_appendindex(char *s, unsigned int i)
{
   while (*s) s++;
   *s++ = '[';
   if (i<10)
       {*s++ = '0'+i; *s++=']'; *s=0; return;}
   if (i<100)
       {*s++ = '0'+i/10; *s++='0'+i%10; *s++=']'; *s=0; return;}
   sprintf(s,"%d]",i);
}

inline long double_to_long(double d)
{
    // gcc feature: if double d=0xc0000000, (long)d yields 0x80000000 !
    // This only happens with long: unsigned long is OK.
    // This causes trouble if we in fact want to cast this long to unsigned long, see NED_expr_2.test.
    // Workaround follows. Note: even the ul variable is needed: when inlining it, gcc will do the wrong cast!
    long l = (long)d;
    unsigned long ul = (unsigned long)d;
    return d<0 ? l : ul;
}

// internal
inline std::string double_to_str(double t)
{
#if __cplusplus >= 201103L
   return std::to_string(t);
#else
   char buf[32];
   return gcvt(t,16,buf);
#endif
}

}  // namespace omnetpp

#endif


