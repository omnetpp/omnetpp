//==========================================================================
//  CMERSENNETWISTER.CC - part of
//                         OMNeT++
//              Discrete System Simulation in C++
//
// Contents:
//   class cLCG32
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


/**
 * Implemements a 32-bit (2^31-2 cycle length) linear congruential random
 * number generator.
 *    - Range:          1 ... 2**31-2
 *    - Period length:  2**31-2
 *    - Method:  x=(x * 7**5) mod (2**31-1)
 *    - Required hardware:  exactly 32-bit integer aritmetics
 *    - To check:  if  x[0]=1  then  x[10000]=1,043,618,065
 *
 * Source:  Raj Jain: The Art of Computer Systems Performance Analysis
 * (John Wiley & Sons, 1991) pp 441-444, 455.
 */
class cLCG32 : public cRNG
{
  protected:
    long seed;

  public:
    cLCG32() {}
    virtual ~cLCG32() {}

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

Register_Class(cLCG32);

#define LCG32_INTRAND_MAX  0x7ffffffeL  /* = 2**31-2 */

void cLCG32::initialize(int runnumber, int id, cConfiguration *cfg)
{
    seed = 0; //FIXME
}

unsigned long cLCG32::intRand()
{
    const long int a=16807, q=127773, r=2836;
    seed=a*(seed%q) - r*(seed/q);
    if (seed<=0) seed+=LCG32_INTRAND_MAX+1;
    return seed;
}

double cLCG32::doubleRand()
{
    return (double)intRand() / (double)((unsigned long)LCG32_INTRAND_MAX+1UL);
}

