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
  Copyright (C) 1992-2003 Andras Varga

  This file is distributed WITHOUT ANY WARRANTY. See the file
  `license' for details on this and other legal matters.
*--------------------------------------------------------------*/

#ifndef __RANDOM_H
#define __RANDOM_H

#include "defs.h"


/**
 * Number of random number generators.
 */
#define NUM_RANDOM_GENERATORS    32

#define INTRAND_MAX  0x7ffffffeL  /* = 2**31-2 FIXME */


/**
 * @name Random number generation.
 *
 * Docs is in the group documentation in index.h. Whatever you type here is
 * ignored by Doxygen :-(
 *
 * @ingroup RandomNumbers
 */
//@{

/**
 * Returns 1 if the random generator works OK. Keeps seed intact.
 * It works by checking the following: starting with x[0]=1,
 * x[10000]=1,043,618,065 must hold.
 */
SIM_API int testrand();

/**
 * Calculates the next random number with the given seed, and also updates
 * the seed. This function makes it possible to use random number streams
 * independent of the seeds built into OMNeT++.
 */
SIM_API long opp_nextrand(long& seed);

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

//==========================================================================
//=== Implementation of utility functions:

inline double dblrand()
{
   return (double)intrand() / (double)((unsigned long)INTRAND_MAX+1UL);
}

inline double genk_dblrand(int gen_nr)
{
   return (double)genk_intrand(gen_nr) / (double)((unsigned long)INTRAND_MAX+1UL);
}

#endif


