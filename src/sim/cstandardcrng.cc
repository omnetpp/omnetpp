//==========================================================================
//  CSTANDARDCRNG.CC - part of
//                         OMNeT++
//              Discrete System Simulation in C++
//
// Contents:
//   class cStandardCRNG
//
//==========================================================================

/*--------------------------------------------------------------*
  Copyright (C) 2002-2004 Andras Varga

  This file is distributed WITHOUT ANY WARRANTY. See the file
  `license' for details on this and other legal matters.
*--------------------------------------------------------------*/

#include <stdlib.h>
#include "cenvir.h"
#include "defs.h"
#include "util.h"
#include "crng.h"
#include "cexception.h"


/**
 * Wraps the standard C random number generator, <tt>rand()</tt>.
 */
class cStandardCRNG : public cRNG
{
  protected:
    long seed;

  public:
    cStandardCRNG() {}
    virtual ~cStandardCRNG() {}

    /**
     * Sets up the RNG.
     */
    virtual void initialize(int runnumber, int id, cConfiguration *cfg);

    /**
     * Invokes C's rand() function. Side effect: built-in generator's seed changed.
     */
    virtual unsigned long intRand();

    /**
     * Relies on intRand() to generate a random number on the [0,1) real interval
     */
    virtual double doubleRand();
};

Register_Class(cStandardCRNG);


void cStandardCRNG::initialize(int runnumber, int id, cConfiguration *cfg)
{
    seed = 0; //FIXME
}

unsigned long cStandardCRNG::intRand()
{
    assert(RAND_MAX==0x7fff);  //FIXME!!! not 0x7fffffff!
    ::srand(seed);
    seed = ::rand();
    return seed;
}

double cStandardCRNG::doubleRand()
{
    return (double)intRand() / (double)((unsigned long)RAND_MAX+1UL);
}

