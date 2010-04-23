//==========================================================================
//  RANDOM.H - part of
//                     OMNeT++/OMNEST
//            Discrete System Simulation in C++
//
//
//  Random number generator functions
//
//==========================================================================

/*--------------------------------------------------------------*
  Copyright (C) 1992-2008 Andras Varga
  Copyright (C) 2006-2008 OpenSim Ltd.

  This file is distributed WITHOUT ANY WARRANTY. See the file
  `license' for details on this and other legal matters.
*--------------------------------------------------------------*/

#ifndef __RANDOM_H
#define __RANDOM_H

#include "simkerneldefs.h"
#include "csimulation.h"
#include "cmodule.h"
#include "crng.h"
#include "cenvir.h"

NAMESPACE_BEGIN


// helper: return the "current" RNG.
inline cRNG *__contextModuleRNG(int k) {
    return simulation.getContextModule() ? simulation.getContextModule()->getRNG(k) : ev.getRNG(k);
}

/**
 * @ingroup RandomNumbers
 * @defgroup RandomNumbersGen Random number generation
 */
//@{

/**
 * Produces a random integer in the range [0,r) using generator 0.
 */
inline long intrand(long r)  {return __contextModuleRNG(0)->intRand(r);}

/**
 * Produces a random double in the range [0,1) using generator 0.
 */
inline double dblrand()  {return __contextModuleRNG(0)->doubleRand();}

/**
 * Produces a random integer in the range [0,r) using generator k.
 */
inline long genk_intrand(int k,long r)  {return __contextModuleRNG(k)->intRand(r);}

/**
 * Produces a random double in the range [0,1) using generator k.
 */
inline double genk_dblrand(int k)  {return __contextModuleRNG(k)->doubleRand();}
//@}

NAMESPACE_END


#endif


