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
  Copyright (C) 1992-2005 Andras Varga

  This file is distributed WITHOUT ANY WARRANTY. See the file
  `license' for details on this and other legal matters.
*--------------------------------------------------------------*/

#ifndef __RANDOM_H
#define __RANDOM_H

#include "defs.h"
#include "csimul.h"
#include "cmodule.h"
#include "crng.h"


// helper: return the "current" RNG.
inline cRNG *__contextModuleRNG(int k) {
    return simulation.contextModule() ? simulation.contextModule()->rng(k) : ev.rng(k);
}

/**
 * @name Random number generation.
 *
 * @ingroup RandomNumbers
 */
//@{

/**
 * Produces random integer in range [0,r) using generator 0.
 */
inline long intrand(long r)  {return __contextModuleRNG(0)->intRand(r);}

/**
 * Produces random double in range [0,1) using generator 0.
 */
inline double dblrand()  {return __contextModuleRNG(0)->doubleRand();}

/**
 * Produces random integer in range [0,r) using generator k.
 */
inline long genk_intrand(int k,long r)  {return __contextModuleRNG(k)->intRand(r);}

/**
 * Produces random double in range [0,1) using generator k.
 */
inline double genk_dblrand(int k)  {return __contextModuleRNG(k)->doubleRand();}
//@}

#endif


