//==========================================================================
//   RANDOM.H - header for
//                             OMNeT++
//            Discrete System Simulation in C++
//
//
//  Random number generator functions
//
//==========================================================================

/*--------------------------------------------------------------*
  Copyright (C) 1992-2004 Andras Varga

  This file is distributed WITHOUT ANY WARRANTY. See the file
  `license' for details on this and other legal matters.
*--------------------------------------------------------------*/

#ifndef __RANDOM_H
#define __RANDOM_H

#include "defs.h"
#include "csimul.h"
#include "cmodule.h"
#include "crng.h"


/**
 * @name Random number generation.
 *
 * @ingroup RandomNumbers
 */
//@{

/**
 * Produces random integer in range [0,r) using generator 0.
 */
inline long intrand(long r)  {return simulation.contextModule()->rng(0)->intRand(r);}

/**
 * Produces random double in range [0,1) using generator 0.
 */
inline double dblrand()  {return simulation.contextModule()->rng(0)->doubleRand();}

/**
 * Produces random integer in range [0,r) using generator k.
 */
inline long genk_intrand(int k,long r)  {return simulation.contextModule()->rng(k)->intRand(r);}

/**
 * Produces random double in range [0,1) using generator k.
 */
inline double genk_dblrand(int k)  {return simulation.contextModule()->rng(k)->doubleRand();}
//@}

#endif


