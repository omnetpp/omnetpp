//==========================================================================
//  CLCG32.CC - part of
//                 OMNeT++/OMNEST
//              Discrete System Simulation in C++
//
//==========================================================================

/*--------------------------------------------------------------*
  Copyright (C) 2002-2017 Andras Varga
  Copyright (C) 2006-2017 OpenSim Ltd.

  This file is distributed WITHOUT ANY WARRANTY. See the file
  `license' for details on this and other legal matters.
*--------------------------------------------------------------*/

#ifndef __OMNETPP_CLCG32_H
#define __OMNETPP_CLCG32_H

#include "simkerneldefs.h"
#include "globals.h"
#include "crng.h"
#include "cconfiguration.h"

namespace omnetpp {


#define LCG32_MAX  0x7ffffffeL  /* = 2^31-2 */

/**
 * @brief Implements a 32-bit (2^31-2 cycle length) linear congruential random
 * number generator.
 *
 *    - Range:              1 ... 2^31-2
 *    - Period length:      2^31-2
 *    - Method:             x=(x * 7^5) mod (2^31-1)
 *    - Required hardware:  exactly 32-bit integer arithmetics
 *    - To check:           if  x[0]=1  then  x[10000]=1,043,618,065
 *
 * Source:  Raj Jain: The Art of Computer Systems Performance Analysis
 * (John Wiley & Sons, 1991) pp 441-444, 455.
 */
class SIM_API cLCG32 : public cRNG
{
  protected:
    int32_t seed;

    // 256 pre-generated seeds, spaced 8,388,608 values in the sequence.
    // This covers the whole RNG period. Enough for 128 runs with 2 RNGs
    // each, or 64 runs with 4 RNGs each -- assuming one run never uses
    // more than 8 million random numbers per RNG.
    static const int32_t autoSeeds[256];

  public:
    cLCG32() {}
    virtual ~cLCG32() {}

    virtual std::string str() const override;

    /** Sets up the RNG. */
    virtual void initialize(int seedSet, int rngId, int numRngs,
                            int parsimProcId, int parsimNumPartitions,
                            cConfiguration *cfg) override;

    /** Tests correctness of the RNG */
    virtual void selfTest() override;

    /** Random integer in the range [0,intRandMax()] */
    virtual uint32_t intRand() override;

    /** Maximum value that can be returned by intRand() */
    virtual uint32_t intRandMax() override;

    /** Random integer in [0,n), n < intRandMax() */
    virtual uint32_t intRand(uint32_t n) override;

    /** Random double on the [0,1) interval */
    virtual double doubleRand() override;

    /** Random double on the (0,1) interval */
    virtual double doubleRandNonz() override;

    /** Random double on the [0,1] interval */
    virtual double doubleRandIncl1() override;
};

}  // namespace omnetpp


#endif


