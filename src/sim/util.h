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
  Copyright (C) 1992,99 Andras Varga
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
// Conversion between simtime_t (=double) and strings like "1s 34ms"
//
// strToSimtime():
//  Returns -1 if the whole string cannot be interpreted as time.
//  Empty string (or spaces+tabs) is also an error.
//  E.g.  "3s 600ms x" --> returns -1.
//
simtime_t strToSimtime(const char *str);

//
// strToSimtime0():
//  Returns time and sets string ptr to terminating zero or to the first
//  character which cannot be interpreted as part of the time string.
//  Empty string is accepted as 0.0.
//  E.g.  "3s 600ms x" --> returns 3.6 and str will point to 'x'.
//
simtime_t strToSimtime0(const char *&str);

//
// simtimeToStr():
//  Converts sim. time into a string like "0.0120000 (12ms)"
//  If no dest ptr is given, uses a static buffer.
//
char *simtimeToStr(simtime_t t, char *dest=NULL);

// equal():
//  Tests equality of two doubles, with the given precision.
//
inline bool equal(double a, double b, double epsilon);

//
// random number generation (they use generator 0)
//
#define myrandomize opp_randomize
void opp_randomize();         // init random number gen. with a random value
long randseed();              // returns current seed
long randseed(long seed);     // sets rndseed and returns old one
int testrand();               // returns 1 if OK; keeps seed intact
long intrand();               // in range 1..INTRAND_MAX
long intrand(long r);         // in range 0..r-1  (OK if r<<INTRAND_MAX)
inline double dblrand();      // in range 0.0..1.0

//
// another set of random number functions (using generator gen_nr)
//
#define genk_myrandomize genk_opp_randomize
void genk_opp_randomize(int gen_nr);
long genk_randseed(int gen_nr);
long genk_randseed(int gen_nr, long seed);
long genk_intrand(int gen_nr);
long genk_intrand(int gen_nr,long r);
inline double genk_dblrand(int gen_nr);

//
// distributions
//
// Argument types and return value must be `double' so that they can be used
// in NED files, and cPar 'F' and 'X' types.
//
double uniform(double a, double b);
double intuniform(double a, double b);
double exponential(double p);
double normal(double m, double d);
double truncnormal(double m, double d);

double genk_uniform(double gen_nr, double a, double b);
double genk_intuniform(double gen_nr, double a, double b);
double genk_exponential(double gen_nr, double p);
double genk_normal(double gen_nr, double m, double d);
double genk_truncnormal(double gen_nr, double m, double d);

//
// utility functions to support nedc-compiled expressions
//
double min(double a, double b);
double max(double a, double b);
double bool_and(double a, double b);
double bool_or(double a, double b);
double bool_xor(double a, double b);
double bool_not(double a);
double bin_and(double a, double b);
double bin_or(double a, double b);
double bin_xor(double a, double b);
double bin_compl(double a);
double shift_left(double a, double b);
double shift_right(double a, double b);


//
// Value-added string functions.
//
//  They also accept NULL pointers (treat them as ptr to "") and use
//  operator new instead of malloc().
//
#define mystrdup    opp_strdup
#define mystrcpy    opp_strcpy
#define mystrcmp    opp_strcmp
#define mystrmatch  opp_strmatch
char *opp_strdup(const char *);
char *opp_strcpy(char *,const char *);
int  opp_strcmp(const char *, const char *);
bool opp_strmatch(const char *, const char *);

// Fast string manipulation functions.
// fastconcat() returns a pointer to a static buffer of length 256
char *fastconcat(const char *s1, const char *s2, const char *s3=NULL, const char *s4=NULL);

// indexedname() creates a string like "component[35]" into buf, the first argument.
char *indexedname(char *buf, const char *name, int index);

// correct(): correct NULL pointer to "" (ptr to a null string)
inline const char *correct(const char *);

// opp_vsscanf(): a restricted vsscanf implementation used by cStatistic::freadvarsf()
int opp_vsscanf(const char *s, const char *fmt, va_list va);

bool memoryIsLow();

//
// Error handling
//
// The functions call simulation.error()/warning(). These functions were
// introduced so that not every class that wants to issue an error message
// needs to include "csimul.h" and half the simulation kernel with it.
//
void opp_error(int errcode,...);         // general error handler
void opp_error(const char *msg,...);     // same w/ custom message
void opp_warning(int errcode,...);       // message + question:continue/abort?
void opp_warning(const char *msg,...);   // same w/ custom message
void opp_terminate(int errcode,...);     // print message and set error number
void opp_terminate(const char *msg,...); // same w/ custom message


//==========================================================================
// Class opp_string.
//   A value-added version of char*. Added value is automatic
//   allocation/deallocation (through opp_strdup/delete.)
//   (It has an own (opp_strdupped) copy of the string)
//
//   Recommended use: as class member, where otherwise the class members
//   would have to call opp_strdup() and 'delete' for a 'char *' ptr.
//
//   Example usage:
//      opp_string a, opp_string b("foo");
//      a = "bar";
//      a = b;
//      const char *s = a;
//      printf("string: `%s'\n", (const char *)a );
//
class opp_string
{
    char *str;
  public:
    opp_string()               {str = 0;}
    opp_string(const char *s)  {str = opp_strdup(s);}
    opp_string(opp_string& s)  {str = opp_strdup(s.str);}
    ~opp_string()              {delete str;}
    operator const char *()    {return str;}
    const char *operator=(const char *s)
                               {delete str;str=opp_strdup(s);return str;}
    opp_string& operator=(opp_string& s)
                               {delete str;str=opp_strdup(s.str);return *this;}
};

//==========================================================================
//=== utility functions:

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
