//==========================================================================
//   UTIL.H - header for
//                             OMNeT++
//            Discrete System Simulation in C++
//
//
//  Utility functions
//
//==========================================================================

/*--------------------------------------------------------------*
  Copyright (C) 1992-2003 Andras Varga

  This file is distributed WITHOUT ANY WARRANTY. See the file
  `license' for details on this and other legal matters.
*--------------------------------------------------------------*/

#ifndef __UTIL_H
#define __UTIL_H

#include <stdarg.h>  // for va_list
#include <typeinfo>  // for type_info
#include "defs.h"


/**
 * Number of random number generators.
 */
#define NUM_RANDOM_GENERATORS    32

#define INTRAND_MAX  0x7ffffffeL  /* = 2**31-2 FIXME */


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
 * E.g. strToSimtime0("3s 600ms x") will return 3.6 and the
 * pointerstr will point to the character 'x'.
 */
SIM_API simtime_t strToSimtime0(const char *&str);

/**
 * Converts simulation time (passed as simtime_t) into a
 * string like "0.0120000 (12ms)". If no destination pointer
 * is given, uses a static buffer.
 */
SIM_API char *simtimeToStr(simtime_t t, char *dest=NULL);
//@}


/**
 * @name Utility functions to support nedc-compiled expressions.
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
 * DEPRECATED: use <code>throw new cException(...)</code> instead!
 *
 * Terminates the simulation with an error message.
 */
SIM_API void opp_error(int errcode,...);

/**
 * DEPRECATED: use <code>throw new cException(...)</code> instead!
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


/**
 * Very simple string class. opp_string has only one data member,
 * a char* pointer. Allocation/deallocation of the contents takes place
 * via opp_strdup() and operator delete
 *
 * Recommended use: as class member, where otherwise the class members
 * would have to call opp_strdup() and delete for the char* member.
 *
 * @ingroup SimSupport
 */
class SIM_API opp_string
{
  private:
    char *str;

  public:
    /**
     * Constructor.
     */
    opp_string()               {str = 0;}

    /**
     * Constructor.
     */
    opp_string(const char *s)  {str = opp_strdup(s);}

    /**
     * Copy constructor.
     */
    opp_string(opp_string& s)  {str = opp_strdup(s.str);}

    /**
     * Destructor.
     */
    ~opp_string()              {delete[] str;}

    /**
     * Returns pointer to the string.
     */
    operator const char *() const    {return str;}

    /**
     * Returns pointer to the internal buffer where the string is stored.
     * It is allowed to write into the string via this pointer, but the
     * length of the string should not be exceeded.
     */
    char *buffer() const        {return str;}

    /**
     * Allocates a buffer of the given size.
     */
    char *allocate(unsigned size)
                               {delete[] str;str=new char[size];return str;}

    /**
     * Deletes the old value and opp_strdup()'s the new value
     * to create the object's own copy.
     */
    const char *operator=(const char *s)
                               {delete[] str;str=opp_strdup(s);return str;}

    /**
     * Assignment.
     */
    opp_string& operator=(const opp_string& s)
                               {delete[] str;str=opp_strdup(s.str);return *this;}
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

#endif


