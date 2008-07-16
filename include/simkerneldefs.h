//==========================================================================
//  SIMKERNELDEFS.H - part of
//                     OMNeT++/OMNEST
//            Discrete System Simulation in C++
//
//
//  Defines of for the simulation kernel
//
//==========================================================================

/*--------------------------------------------------------------*
  Copyright (C) 1992-2008 Andras Varga
  Copyright (C) 2006-2008 OpenSim Ltd.

  This file is distributed WITHOUT ANY WARRANTY. See the file
  `license' for details on this and other legal matters.
*--------------------------------------------------------------*/

#ifndef __SIMKERNELDEFS_H
#define __SIMKERNELDEFS_H

#include <stddef.h>
#include <float.h>
#include <math.h>
#include "platdep/platdefs.h"
#include "platdep/intxtypes.h"

// OMNeT++ version -- must match NEDC_VERSION and MSGC_VERSION in nedxml!
//XXX move to platdefs.h? and remove NEDC_VERSION
#define OMNETPP_VERSION 0x0400

#if defined(SIM_EXPORT)
#  define SIM_API OPP_DLLEXPORT
#elif defined(SIM_IMPORT) || defined(OMNETPPLIBS_IMPORT)
#  define SIM_API OPP_DLLIMPORT
#else
#  define SIM_API
#endif

// NULL
#ifndef NULL
#define NULL ((void*)0)
#endif

//
// other common defines
//

#ifndef PI
#define PI        3.141592653589793
#endif

#ifndef Min
#define Min(a,b)     ( (a)<(b) ? (a) : (b) )
#define Max(a,b)     ( (a)>(b) ? (a) : (b) )
#endif

#define sgn(x)       ((x)==0 ? 0 : ((x)<0 ? -1 : 1))


#ifndef NDEBUG
#define ASSERT(expr) \
  ((void) ((expr) ? 0 : (opp_error("ASSERT: condition %s false in function %s, %s line %d", \
                                   #expr, __FUNCTION__, __FILE__, __LINE__), 0)))
#define ASSERT2(expr,text) \
  ((void) ((expr) ? 0 : (opp_error("ASSERT: %s in function %s, %s line %d", \
                                   text, __FUNCTION__, __FILE__, __LINE__), 0)))
#else
#define ASSERT(expr)        ((void)0)
#define ASSERT2(expr,text)  ((void)0)
#endif


typedef int64 eventnumber_t;


/**
 * Prototype for functions that are called by some objects (cMsgPar, cLinkedList)
 * to free up user-defined data structures.
 * @ingroup EnumsTypes
 */
typedef void (*VoidDelFunc)(void *);

/**
 * Prototype for functions that are called by some objects (cMsgPar, cLinkedList)
 * to duplicate user-defined data structures.
 * @ingroup EnumsTypes
 */
typedef void *(*VoidDupFunc)(void *);


#endif

