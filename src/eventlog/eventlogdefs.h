//=========================================================================
//  EVENTLOGDEFS.H - part of
//                  OMNeT++/OMNEST
//           Discrete System Simulation in C++
//
//=========================================================================

/*--------------------------------------------------------------*
  Copyright (C) 1992-2005 Andras Varga

  This file is distributed WITHOUT ANY WARRANTY. See the file
  `license' for details on this and other legal matters.
*--------------------------------------------------------------*/

#ifndef _EVENTLOGDEFS_H_
#define _EVENTLOGDEFS_H_

#include "exception.h"

// define int64, our equivalent of Java's "long" type
#ifdef _MSC_VER
typedef __int64 int64;
#else
typedef long long int64;
#endif

#ifndef NDEBUG
#define EASSERT(expr)  \
  ((void) ((expr) ? 0 : \
           (throw new Exception("ASSERT: condition %s false, %s line %d", \
                             #expr, __FILE__, __LINE__), 0)))
#else
#define ASSERT(expr)  ((void)0)
#endif

#endif
