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
  Copyright (C) 1992-2001 Andras Varga
  Technical University of Budapest, Dept. of Telecommunications,
  Stoczek u.2, H-1111 Budapest, Hungary.

  This file is distributed WITHOUT ANY WARRANTY. See the file
  `license' for details on this and other legal matters.
*--------------------------------------------------------------*/

#ifndef __UTIL_H
#define __UTIL_H

#include <stdarg.h>  // for va_list
#include "defs.h"


#define NUM_RANDOM_GENERATORS    32

#define INTRAND_MAX  0x7ffffffeL  /* = 2**31-2 */

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

/**
 * @name Converting simulation time to and from string form.
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
 * @name Random number generation.
 *
 * OMNeT++ has a built-in pseudorandom number generator that gives long int
 * (32-bit) values in the range 1...2^31-2, with a period length of 2^31-2.
 *
 * The generator is a linear congruential generator (LCG), and uses the method
 * x=(x * 75) mod (2^31-1). The testrand() method can be used
 * to check if the generator works correctly. Required hardware is exactly
 * 32-bit integer arithmetics.
 *
 * OMNeT++ provides several independent random number generators
 * (by default 32; this number is #defined as NUM_RANDOM_GENERATORS in
 * utils.h), identified by numbers. The generator number is usually the gen_nr
 * argument to functions beginning with genk_.
 *
 * Source: Raj Jain: The Art of Computer Systems Performance Analysis
 * (John Wiley & Sons, 1991), pages 441-444, 455.
 */
//@{

/**
 * Returns 1 if the random generator works OK. Keeps seed intact.
 * It works by checking the following: starting with x[0]=1,
 * x[10000]=1,043,618,065 must hold.
 */
SIM_API int testrand();

/**
 * Initialize random number generator 0 with a random value.
 */

SIM_API void opp_randomize();

/**
 * Returns current seed of generator 0.
 */
SIM_API long randseed();

/**
 * Sets seed of generator 0 and returns old seed value. Zero is not allowed as a seed.
 */
SIM_API long randseed(long seed);

/**
 * Produces random integer in the range 1...INTRAND_MAX using generator 0.
 */
SIM_API long intrand();

/**
 * Produces random integer in range 0...r-1 using generator 0.  (Assumes r &lt;&lt; INTRAND_MAX.)
 */
SIM_API long intrand(long r);

/**
 * Produces random double in range 0.0...1.0 using generator 0.
 */
inline  double dblrand();

/**
 * Initialize random number generator gen_nr with a random value.
 */
SIM_API void genk_opp_randomize(int gen_nr);

/**
 * Returns current seed of generator gen_nr.
 */
SIM_API long genk_randseed(int gen_nr);

/**
 * Sets seed of generator gen_nr and returns old seed value. Zero is not allowed as a seed.
 */
SIM_API long genk_randseed(int gen_nr, long seed);

/**
 * Produces random integer in the range 1...INTRAND_MAX using generator gen_nr.
 */
SIM_API long genk_intrand(int gen_nr);

/**
 * Produces random integer in range 0...r-1 using generator gen_nr. (Assumes r &lt;&lt; INTRAND_MAX.)
 */
SIM_API long genk_intrand(int gen_nr,long r);

/**
 * Produces random double in range 0.0...1.0 using generator gen_nr.
 */
inline  double genk_dblrand(int gen_nr);
//@}


/**
 * @name Distributions.
 *
 * Argument types and return value must be `double' so that they can be used
 * in NED files, and cPar 'F' and 'X' types.
 */
//@{

/**
 *
 */
SIM_API double uniform(double a, double b);

/**
 *
 */
SIM_API double intuniform(double a, double b);

/**
 *
 */
SIM_API double exponential(double p);

/**
 *
 */
SIM_API double normal(double m, double d);

/**
 *
 */
SIM_API double truncnormal(double m, double d);


/**
 * Same as the function without the genk_ prefix, only uses random generator
 * gen_nr instead of generator 0.
 */
SIM_API double genk_uniform(double gen_nr, double a, double b);

/**
 * Same as the function without the genk_ prefix, only uses random generator
 * gen_nr instead of generator 0.
 */
SIM_API double genk_intuniform(double gen_nr, double a, double b);

/**
 * Same as the function without the genk_ prefix, only uses random generator
 * gen_nr instead of generator 0.
 */
SIM_API double genk_exponential(double gen_nr, double p);

/**
 * Same as the function without the genk_ prefix, only uses random generator
 * gen_nr instead of generator 0.
 */
SIM_API double genk_normal(double gen_nr, double m, double d);

/**
 * Same as the function without the genk_ prefix, only uses random generator
 * gen_nr instead of generator 0.
 */
SIM_API double genk_truncnormal(double gen_nr, double m, double d);
//@}


/**
 * Utility functions to support nedc-compiled expressions.
 */
//@{

/**
 *
 */
SIM_API double min(double a, double b);

/**
 *
 */
SIM_API double max(double a, double b);

/**
 *
 */
SIM_API double bool_and(double a, double b);

/**
 *
 */
SIM_API double bool_or(double a, double b);

/**
 *
 */
SIM_API double bool_xor(double a, double b);

/**
 *
 */
SIM_API double bool_not(double a);

/**
 *
 */
SIM_API double bin_and(double a, double b);

/**
 *
 */
SIM_API double bin_or(double a, double b);

/**
 *
 */
SIM_API double bin_xor(double a, double b);

/**
 *
 */
SIM_API double bin_compl(double a);

/**
 *
 */
SIM_API double shift_left(double a, double b);

/**
 *
 */
SIM_API double shift_right(double a, double b);
//@}


/**
 * Value-added string functions.
 *
 *  They also accept NULL pointers (treat them as ptr to "") and use
 *  operator new instead of malloc().
 */
//@{

/**
 *
 */
SIM_API char *opp_strdup(const char *);

/**
 *
 */
SIM_API char *opp_strcpy(char *,const char *);

/**
 *
 */
SIM_API int  opp_strcmp(const char *, const char *);

/**
 *
 */
SIM_API bool opp_strmatch(const char *, const char *);
//@}

/**
 * @name Miscellaneous functions.
 */
//@{

/**
 * Concatentates up to four strings. Returns a pointer to a static buffer of length 256.
 */
SIM_API char *opp_concat(const char *s1, const char *s2, const char *s3=NULL, const char *s4=NULL);

/**
 * Creates a string like "component[35]" into buf, the first argument.
 */
SIM_API char *indexedname(char *buf, const char *name, int index);

/**
 * Correct NULL pointer to "" (ptr to a null string).
 */
inline const char *correct(const char *);

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
 * Error handling to be used in new classes. The functions call
 * simulation.error()/warning(). These functions were introduced so
 * that not every class that wants to issue an error message
 * needs to include "csimul.h" and half the simulation kernel with it.
 */
//@{

/**
 * Terminates the simulation with an error message.
 */
SIM_API void opp_error(int errcode,...);

/**
 * Same as function with the same name, but using custom message string.
 * To be called like printf().
 */
SIM_API void opp_error(const char *msg,...);

/**
 * Message + question: continue or abort?
 */
SIM_API void opp_warning(int errcode,...);

/**
 * Same as function with the same name, but using custom message string.
 * To be called like printf().
 */
SIM_API void opp_warning(const char *msg,...);

/**
 * Print message and set error number.
 */
SIM_API void opp_terminate(int errcode,...);

/**
 * Same as function with the same name, but using custom message string.
 * To be called like printf().
 */
SIM_API void opp_terminate(const char *msg,...);
//@}

/**
 * Very simple string class. opp_string has only one data member,
 * a char* pointer. Allocation/deallocation of the contents takes place
 * via opp_strdup() and operator delete
 *
 * Recommended use: as class member, where otherwise the class members
 * would have to call opp_strdup() and delete for the char* member.
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
    ~opp_string()              {delete str;}

    /**
     * Returns pointer to the string.
     */
    operator const char *()    {return str;}

    /**
     * MISSINGDOC: opp_string:char*buffer()
     */
    char *buffer()             {return str;}

    /**
     * Deletes the old value and opp_strdup()'s the new value
     * to create the object's own copy.
     */
    const char *operator=(const char *s)
                               {delete str;str=opp_strdup(s);return str;}

    /**
     * Assignment.
     */
    opp_string& operator=(opp_string& s)
                               {delete str;str=opp_strdup(s.str);return *this;}
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

inline double dblrand()
{
   return (double)intrand() / (double)((unsigned long)INTRAND_MAX+1UL);
}

inline double genk_dblrand(int gen_nr)
{
   return (double)genk_intrand(gen_nr) / (double)((unsigned long)INTRAND_MAX+1UL);
}

#endif


