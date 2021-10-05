//==========================================================================
//  CMERSENNETWISTER.CC - part of
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

#ifndef __OMNETPP_CMERSENNETWISTER_H
#define __OMNETPP_CMERSENNETWISTER_H

#include "simkerneldefs.h"
#include "globals.h"
#include "crng.h"
#include "cconfiguration.h"
#include "mersennetwister.h"

namespace omnetpp {


/**
 * @brief Wraps the Mersenne Twister RNG by Makoto Matsumoto and Takuji Nishimura.
 *
 * Mersenne Twister has a cycle length is 2^19937-1, and 623-dimensional
 * equidistribution property is assured.
 *
 * http://www.math.sci.hiroshima-u.ac.jp/~m-mat/MT/ewhat-is-mt.html
 *
 * Actual code used is MersenneTwister.h from Richard J. Wagner,
 * v1.0, 15 May 2003, rjwagner@writeme.com.
 *
 * http://www-personal.engin.umich.edu/~wagnerr/MersenneTwister.html
 */
class SIM_API cMersenneTwister : public cRNG
{
  protected:
    MTRand rng;

  public:
    cMersenneTwister() {}
    virtual ~cMersenneTwister() {}

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

