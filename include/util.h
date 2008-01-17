//==========================================================================
//  UTIL.H - part of
//                     OMNeT++/OMNEST
//            Discrete System Simulation in C++
//
//
//  Utility functions
//
//==========================================================================

/*--------------------------------------------------------------*
  Copyright (C) 1992-2005 Andras Varga

  This file is distributed WITHOUT ANY WARRANTY. See the file
  `license' for details on this and other legal matters.
*--------------------------------------------------------------*/

#ifndef __UTIL_H
#define __UTIL_H

#include <stdarg.h>  // for va_list
#include <stdio.h>   // for sprintf
#include <typeinfo>  // for type_info
#include "simkerneldefs.h"
#include "errmsg.h"

NAMESPACE_BEGIN

// forward declarations
class cComponent;

// logically belongs to csimul.h but must be here because of declaration order
enum {CTX_BUILD, CTX_INITIALIZE, CTX_EVENT, CTX_FINISH};

// windows.h defines min() and max() as macros
#undef min
#undef max

#ifdef USE_DOUBLE_SIMTIME
//FIXME deprecate
/**
 * @name Converting simulation time to and from string form.
 * @ingroup Functions
 */
//@{

/**
 * Convert a string to simtime_t. The string should have a format
 * similar to the one output by simtimeToStr() (like "1s 34ms").
 *
 * Returns -1 if the whole string cannot be interpreted as time.
 * Empty string (or only spaces+tabs) is also an error.
 * E.g. strtoSimtime("3s 600ms x") will return -1.
 */
SIM_API double strToSimtime(const char *str);

/**
 * Convert the beginning of a string to simtime_t. Similar to
 * strToSimtime(), only it processes the string as far as it
 * can be interpreted as simulation time. It sets the pointer
 * passed to the first character which cannot be interpreted
 * as part of the time string, or to the terminating zero.
 * Empty string is accepted as 0.0.
 *
 * Example: strToSimtime0("3s 600ms x") will return 3.6 and the
 * str pointer will point to the character 'x'.
 */
SIM_API double strToSimtime0(const char *&str);

/**
 * Converts simulation time (passed as simtime_t) into a
 * string like "0.0120000 (12ms)". If no destination pointer
 * is given, it will use a static buffer.
 */
SIM_API char *simtimeToStr(double t, char *dest=NULL);

/**
 * Converts simulation time (passed as simtime_t) into a short string
 * form like "12.37ms". If no destination pointer is given, it will use
 * a static buffer.
 */
SIM_API char *simtimeToStrShort(double t, char *buf=NULL);
//@}
#endif //USE_DOUBLE_SIMTIME


/**
 * @name Utility functions to support nedtool-compiled expressions.
 * @ingroup Functions
 */
//@{

/**
 * Returns the minimum of a and b.
 */
SIM_API double min(double a, double b);

/**
 * Returns the maximum of a and b.
 */
SIM_API double max(double a, double b);
//@}


/**
 * @name String-related utility functions.
 *
 * Some of these functions are similar to <string.h> functions, with the
 * exception that they also accept NULL pointers as empty strings (""),
 * and use operator new instead of malloc(). It is recommended to use these
 * functions instead of the original <string.h> functions.
 *
 * @ingroup Functions
 */
//@{

/**
 * Duplicates the string. If the pointer passed is NULL or points
 * to a null string (""), NULL is returned.
 */
SIM_API char *opp_strdup(const char *);

/**
 * Same as the standard strcpy() function, except that NULL pointers
 * in the second argument are treated like pointers to a null string ("").
 */
SIM_API char *opp_strcpy(char *,const char *);

/**
 * Same as the standard strcmp() function, except that NULL pointers
 * are treated like pointers to a null string ("").
 */
SIM_API int  opp_strcmp(const char *, const char *);

/**
 * Returns true if the two strings are identical up to the length of the
 * shorter one. NULL pointers are treated like pointers to a null string ("").
 */
SIM_API bool opp_strmatch(const char *, const char *);

/**
 * Same as the standard strlen() function, except that does not crash
 * on NULL pointers but returns 0.
 */
SIM_API int opp_strlen(const char *);

/**
 * Creates a string like "component[35]" into dest, the first argument.
 */
SIM_API char *opp_mkindexedname(char *dest, const char *name, int index);

/**
 * Concatenates up to four strings. Returns a pointer to a static buffer
 * of length 256. If the result length would exceed 256, it is truncated.
 */
SIM_API char *opp_concat(const char *s1, const char *s2, const char *s3=NULL, const char *s4=NULL);

/**
 * Copies src string into desc, and if its length would exceed maxlen,
 * it is truncated with an ellipsis. For example, <tt>opp_strprettytrunc(buf,
 * "long-long",6)</tt> yields <tt>"lon..."</tt>.
 */
SIM_API char *opp_strprettytrunc(char *dest, const char *src, unsigned maxlen);

/**
 * Returns the pointer passed as argument unchanged, except that if it was NULL,
 * it returns a pointer to a null string ("").
 */
inline const char *opp_nulltoempty(const char *);
//@}

/**
 * @name Miscellaneous functions.
 * @ingroup Functions
 */
//@{

/**
 * Tests equality of two doubles, with the given precision.
 */
inline bool equal(double a, double b, double epsilon);
//@}

//
// INTERNAL: a restricted vsscanf implementation used by cStatistic::freadvarsf()
//
SIM_API int opp_vsscanf(const char *s, const char *fmt, va_list va);

/**
 * DEPRECATED: Error handling functions.
 */
//@{

/**
 * DEPRECATED: use <tt>throw cRuntimeError(...)</tt> instead!
 *
 * Terminates the simulation with an error message.
 */
SIM_API void opp_error(ErrorCode errcode,...);

/**
 * DEPRECATED: use <tt>throw cRuntimeError(...)</tt> instead!
 *
 * Same as function with the same name, but using custom message string.
 * To be called like printf().
 */
SIM_API void opp_error(const char *msg,...);

/**
 * This method can be used to report non-fatal discrepancies to the user.
 * Generally, warnings should VERY RARELY be used, if ever.
 * Argument list is like printf().
 *
 * Unlike in earlier versions, the user will NOT be offered the possibility
 * to stop the simulation. (In Cmdenv, warnings will be written
 * to the standard error, and in Tkenv it will probably pop up an
 * [OK] dialog.
 */
SIM_API void opp_warning(ErrorCode errcode,...);

/**
 * This method can be used to report non-fatal discrepancies to the user.
 * Generally, warnings should VERY RARELY be used, if ever.
 * Argument list works like printf().
 *
 * Unlike in earlier versions, the user will NOT be offered the possibility
 * to stop the simulation. (In Cmdenv, warnings will be written
 * to the standard error, and in Tkenv it will probably pop up an
 * [OK] dialog.
 */
SIM_API void opp_warning(const char *msg,...);

/**
 * DEPRECATED.
 *
 * Print message and set error number.
 */
SIM_API void opp_terminate(ErrorCode errcode,...);

/**
 * DEPRECATED.
 *
 * Same as function with the same name, but using custom message string.
 * To be called like printf().
 */
SIM_API void opp_terminate(const char *msg,...);
//@}

//==========================================================================

// INTERNAL: return name of a C++ type -- correcting quirks of various compilers.
SIM_API const char *opp_typename(const std::type_info& t);

//==========================================================================

#define MAX_METHODCALL 1024

/**
 * Denotes module class member function as callable from other modules.
 *
 * Usage: <tt>Enter_Method("getRoutingTable(node=%d)",node);<tt>
 *
 * The macro should be put at the top of every module member function
 * that may be called from other modules. This macro arranges to
 * temporarily switch the context to this module (the old context
 * will be restored automatically when the method returns),
 * and also lets the graphical user interface animate the method call.
 *
 * The argument(s) should specify the method name (and parameters) --
 * it will be used for the animation. The argument list works as in
 * <tt>printf()</tt>, so it's easy to include the actual parameter values.
 *
 * @see Enter_Method_Simple() macro
 */
#define Enter_Method cMethodCallContextSwitcher __ctx(this, false); __ctx.methodCall

/**
 * Denotes module class member function as callable from other modules.
 * This macro is similar to the Enter_Method() macro, only it does
 * not do animation and thus it doesn't expect the methods name as
 * argument.
 *
 * Usage: <tt>Enter_Method_Silent();<tt>
 *
 * @see Enter_Method() macro
 */
#define Enter_Method_Silent() cMethodCallContextSwitcher __ctx(this, true)

/**
 * The constructor switches the context to the given component, and the
 * destructor restores the original context.
 *
 * @see cSimulation::contextModule(), cSimulation::setContextModule()
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
    cContextSwitcher(cComponent *newContext);

    /**
     * Restores the original context
     */
    ~cContextSwitcher();
};

/**
 * Internal class. May only be used via the Enter_Method() and Enter_Method_Silent() macros!
 * @ingroup Internals
 */
class SIM_API cMethodCallContextSwitcher : public cContextSwitcher
{
  public:
    /**
     * Switches context to the given module
     */
    cMethodCallContextSwitcher(cComponent *newContext, bool notifyEnvir=true);

    /**
     * Restores the original context
     */
    ~cMethodCallContextSwitcher();

    /**
     * Tells the user interface about the method call (so that it can be
     * animated, etc.)
     */
    void methodCall(const char *fmt,...);
};

/**
 * The constructor switches the context type, and the destructor restores
 * the original context type.
 *
 * @see cSimulation::contextModule(), cSimulation::setContextModule()
 * @ingroup Internals
 */
class SIM_API cContextTypeSwitcher
{
  private:
    int contexttype;
  public:

    /**
     * Switches the context type (see CTX_xxx constants)
     */
    cContextTypeSwitcher(int ctxtype);

    /**
     * Restores the original context type
     */
    ~cContextTypeSwitcher();
};

//==========================================================================
//=== Implementation of utility functions:

inline bool equal(double a, double b, double epsilon)
{
   double d = a-b;
   return (d>=0.0 ? d : -d) < epsilon;
}

inline const char *opp_nulltoempty(const char *s)
{
   return s ? s : "";
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

NAMESPACE_END


#endif


