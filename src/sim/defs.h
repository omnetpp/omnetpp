//==========================================================================
//   DEFS.H - header for
//                             OMNeT++
//            Discrete System Simulation in C++
//
//
//  Defines of global interest
//
//==========================================================================

/*--------------------------------------------------------------*
  Copyright (C) 1992,99 Andras Varga
  Technical University of Budapest, Dept. of Telecommunications,
  Stoczek u.2, H-1111 Budapest, Hungary.

  This file is distributed WITHOUT ANY WARRANTY. See the file
  `license' for details on this and other legal matters.
*--------------------------------------------------------------*/

#ifndef __DEFS_H
#define __DEFS_H

#include <stddef.h>   // size_t
#include <math.h>     // HUGE_VAL


//=== NULL
#ifndef NULL
# define NULL ((void*)0)
#endif

#define NO(cXX)   ((cXX *)NULL)

//=== other common defines

#ifndef TRUE
# define TRUE      true
# define FALSE     false
#endif

//// old compilers which don't know bool are no longer supported
// #if (defined(__BORLANDC__) && (__BORLANDC__<=0x410)) || (defined(__WATCOMC__) && (__WATCOMC__<1060))
// typedef int bool;
// #define true   1
// #define false  0
// #endif

#ifndef PI
#define PI        3.141592653589793
#endif

#ifndef Min
#define Min(a,b)     ( (a)<(b) ? (a) : (b) )
#define Max(a,b)     ( (a)>(b) ? (a) : (b) )
#endif

#define sgn(x)       ((x)==0 ? 0 : ((x)<0 ? -1 : 1))

#ifndef NDEBUG
#define ASSERT(expr)  \
  ((void) ((expr) ? 0 : \
           (opp_error("ASSERT: condition %s false, %s line %d", \
                             #expr, __FILE__, __LINE__), 0)))
#else
#define ASSERT(expr)  ((void)0)
#endif

#define CONST_CAST(x)  (char *)(x)

//=== model time
typedef double       simtime_t;
#define MAXTIME      HUGE_VAL

//=== backwards compatibility defines
#define isA()          className()
// cQueue:
#define insertHead(a)  insert(a)
#define peekTail()     tail()
#define peekHead()     head()
#define getTail()      pop()


//=== memory mgmt functions for void* pointers
//    (used by cLinkedList and cPar)
typedef void (*VoidDelFunc)(void *);
typedef void *(*VoidDupFunc)(void *);

//=== used by cPar expressions
typedef double (*MathFunc)(...);
typedef double (*MathFuncNoArg)();
typedef double (*MathFunc1Arg)(double);
typedef double (*MathFunc2Args)(double,double);
typedef double (*MathFunc3Args)(double,double,double);

#endif
