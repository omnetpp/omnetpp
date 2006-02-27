//==========================================================================
//  CLCG32.CC - part of
//                 OMNeT++/OMNEST
//              Discrete System Simulation in C++
//
// Contents:
//   class cLCG32
//
//==========================================================================

/*--------------------------------------------------------------*
  Copyright (C) 2002-2005 Andras Varga

  This file is distributed WITHOUT ANY WARRANTY. See the file
  `license' for details on this and other legal matters.
*--------------------------------------------------------------*/

#ifndef __CLCG32_H
#define __CLCG32_H

#include "defs.h"
#include "macros.h"
#include "crng.h"
#include "cconfig.h"


#define LCG32_MAX  0x7ffffffeL  /* = 2^31-2 */

/**
 * Implements a 32-bit (2^31-2 cycle length) linear congruential random
 * number generator.
 *    - Range:              1 ... 2^31-2
 *    - Period length:      2^31-2
 *    - Method:             x=(x * 7^5) mod (2^31-1)
 *    - Required hardware:  exactly 32-bit integer aritmetics
 *    - To check:           if  x[0]=1  then  x[10000]=1,043,618,065
 *
 * Source:  Raj Jain: The Art of Computer Systems Performance Analysis
 * (John Wiley & Sons, 1991) pp 441-444, 455.
 */
class SIM_API cLCG32 : public cRNG
{
  protected:
    long seed;

    // 256 pre-generated seeds, spaced 8,388,608 values in the sequence.
    // This covers the whole RNG period. Enough for 128 runs with 2 RNGs
    // each, or 64 runs with 4 RNGs each -- assuming one run never uses
    // more than 8 million random numbers per RNG.
    static long autoSeeds[256];

  public:
    cLCG32() {}
    virtual ~cLCG32() {}

    /** Sets up the RNG. */
    virtual void initialize(int runNumber, int rngId, int numRngs,
                            int parsimProcId, int parsimNumPartitions,
                            cConfiguration *cfg);

    /** Tests correctness of the RNG */
    virtual void selfTest();

    /** Random integer in the range [0,intRandMax()] */
    virtual unsigned long intRand();

    /** Maximum value that can be returned by intRand() */
    virtual unsigned long intRandMax();

    /** Random integer in [0,n), n < intRandMax() */
    virtual unsigned long intRand(unsigned long n);

    /** Random double on the [0,1) interval */
    virtual double doubleRand();

    /** Random double on the (0,1) interval */
    virtual double doubleRandNonz();

    /** Random double on the [0,1] interval */
    virtual double doubleRandIncl1();
};

#endif


