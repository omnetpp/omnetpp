//=========================================================================
//  UTIL.H - part of
//                  OMNeT++/OMNEST
//           Discrete System Simulation in C++
//
//=========================================================================

/*--------------------------------------------------------------*
  Copyright (C) 1992-2005 Andras Varga

  This file is distributed WITHOUT ANY WARRANTY. See the file
  `license' for details on this and other legal matters.
*--------------------------------------------------------------*/

#ifndef _UTIL_H_
#define _UTIL_H_

#include <assert.h>
#include "exception.h"

#ifdef NDEBUG
#  define ASSERT(x)
#  define DBG(x)
#else
#  define ASSERT(expr)  ((void) ((expr) ? 0 : (throw opp_runtime_error("ASSERT: condition %s false, %s line %d", \
                        #expr, __FILE__, __LINE__), 0)))
//#  define DBG(x)  printf x
#  define DBG(x)
#endif

#ifdef _WIN32
#ifndef vsnprintf
#define vsnprintf _vsnprintf
#endif
#endif

#endif


