//==========================================================================
//  CMERSENNETWISTER.CC - part of
//                         OMNeT++
//              Discrete System Simulation in C++
//
// Contents:
//   class cMersenneTwister
//
//==========================================================================

/*--------------------------------------------------------------*
  Copyright (C) 2002-2004 Andras Varga

  This file is distributed WITHOUT ANY WARRANTY. See the file
  `license' for details on this and other legal matters.
*--------------------------------------------------------------*/

#include "cenvir.h"
#include "defs.h"
#include "util.h"
#include "crng.h"
#include "cexception.h"
#include "mersennetwister.h"


/**
 * Wraps the Mersenne Twister RNG by Makoto Matsumoto and Takuji Nishimura.
 * Cycle length is 2^19937-1, and 623-dimensional equidistribution property
 * is assured.
 *
 * http://www.math.sci.hiroshima-u.ac.jp/~m-mat/MT/ewhat-is-mt.html
 */
class cMersenneTwister : public cRNG
{
  protected:
    MTRand rng;

  public:
    cMersenneTwister() {}
    virtual ~cMersenneTwister() {}

    /**
     * Sets up the RNG.
     */
    virtual void initialize(int runnumber, int id, cConfiguration *cfg);

    /**
     * Generates a random number on the interval [0,0x7fffffff]
     */
    virtual unsigned long intRand();

    /**
     * Generates a random number on the [0,1) real interval
     */
    virtual double doubleRand();
};

Register_Class(cMersenneTwister);


void cMersenneTwister::initialize(int runnumber, int id, cConfiguration *cfg)
{
    unsigned int seed = 0; //FIXME
    rng.seed(seed);
}

unsigned long cMersenneTwister::intRand()
{
    return rng.randExc();
}

double cMersenneTwister::doubleRand()
{
    useThisRNG();
    return mt19937_genrand_real2();
}

