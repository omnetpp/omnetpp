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
#include "defs.h"

// forward declarations
class cModule;

// logically belongs to csimul.h but must be here because of declaration order
enum {CTX_BUILD, CTX_INITIALIZE, CTX_EVENT, CTX_FINISH};

//
// #defines provided for backwards compatibility.
// They may be removed in a future release!
//
#define myrandomize      opp_randomize
#define genk_myrandomize genk_opp_randomize
#define mystrdup         opp_strdup
#define mystrcpy         opp_strcpy
#define mystrcmp         opp_strcmp
#define mystrmatch       opp_strmatch
#define fastconcat       opp_concat
#define indexedname      opp_mkindexedname

// windows.h defines min() and max() as macros
#undef min
#undef max

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
SIM_API simtime_t strToSimtime(const char *str);

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
SIM_API simtime_t strToSimtime0(const char *&str);

/**
 * Converts simulation time (passed as simtime_t) into a
 * string like "0.0120000 (12ms)". If no destination pointer
 * is given, it will use a static buffer.
 */
SIM_API char *simtimeToStr(simtime_t t, char *dest=NULL);

/**
 * Converts simulation time (passed as simtime_t) into a short string
 * form like "12.37ms". If no destination pointer is given, it will use
 * a static buffer.
 */
SIM_API char *simtimeToStrShort(simtime_t t, char *buf=NULL);
//@}


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

/**
 * Returns the boolean AND of a and b.
 * (Any nonzero number is treated as true.)
 */
SIM_API double bool_and(double a, double b);

/**
 * Returns the boolean OR of a and b.
 * (Any nonzero number is treated as true.)
 */
SIM_API double bool_or(double a, double b);

/**
 * Returns the boolean Exclusive OR of a and b.
 * (Any nonzero number is treated as true.)
 */
SIM_API double bool_xor(double a, double b);

/**
 * Returns the boolean negation of a.
 * (Any nonzero number is treated as true.)
 */
SIM_API double bool_not(double a);

/**
 * Returns the binary AND of a and b.
 * (a and b are converted to unsigned long for the operation.)
 */
SIM_API double bin_and(double a, double b);

/**
 * Returns the binary OR of a and b.
 * (a and b are converted to unsigned long for the operation.)
 */
SIM_API double bin_or(double a, double b);

/**
 * Returns the binary exclusive OR of a and b.
 * (a and b are converted to unsigned long for the operation.)
 */
SIM_API double bin_xor(double a, double b);

/**
 * Returns the bitwise negation (unary complement) of a.
 * (a is converted to unsigned long for the operation.)
 */
SIM_API double bin_compl(double a);

/**
 * Shifts a b bits to the left.
 * (a and b are converted to unsigned long for the operation.)
 */
SIM_API double shift_left(double a, double b);

/**
 * Shifts a b bits to the right.
 * (a and b are converted to unsigned long for the operation.)
 */
SIM_API double shift_right(double a, double b);
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
inline const char *correct(const char *);
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
 * DEPRECATED: use <tt>throw new cRuntimeError(...)</tt> instead!
 *
 * Terminates the simulation with an error message.
 */
SIM_API void opp_error(int errcode,...);

/**
 * DEPRECATED: use <tt>throw new cRuntimeError(...)</tt> instead!
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
SIM_API void opp_warning(int errcode,...);

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
SIM_API void opp_terminate(int errcode,...);

/**
 * DEPRECATED.
 *
 * Same as function with the same name, but using custom message string.
 * To be called like printf().
 */
SIM_API void opp_terminate(const char *msg,...);
//@}

//
// INTERNAL: return name of a C++ type -- correcting quirks of various compilers...
//
const char *opp_typename(const std::type_info& t);

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
#define Enter_Method cContextSwitcher __ctx(this); __ctx.methodCall

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
#define Enter_Method_Silent() cContextSwitcher __ctx(this)

/**
 * Helper class, used internally by the Enter_Method() and Enter_Method_Silent()
 * macros. The macro should be put at the top of module methods that
 * may be called from other modules, and it creates an instance of this class.
 * The constructor switches the context to the module containing the method,
 * and the destructor restores the original context.
 *
 * @see cSimulation::contextModule(), cSimulation::setContextModule()
 * @ingroup Internals
 */
class SIM_API cContextSwitcher
{
  private:
    cModule *callerContext;
  public:
    /**
     * Switches context to the given module
     */
    cContextSwitcher(cModule *thisptr);

    /**
     * Restores the original context
     */
    ~cContextSwitcher();

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

inline const char *correct(const char *s)
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

#endif


