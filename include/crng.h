//==========================================================================
//   CRNG.H   - header for
//                             OMNeT++
//            Discrete System Simulation in C++
//
//
//  cRNG: interface for random number generator classes
//
//==========================================================================

/*--------------------------------------------------------------*
  Copyright (C) 1992-2004 Andras Varga

  This file is distributed WITHOUT ANY WARRANTY. See the file
  `license' for details on this and other legal matters.
*--------------------------------------------------------------*/

#ifndef __CRNG_H
#define __CRNG_H

#include "defs.h"
#include "cpolymorphic.h"

/**
 * Interface for random number generator classes
 */
class SIM_API cRNG : public cPolymorphic
{
  public:
    cRNG() {}
    virtual ~cRNG() {}

    /**
     * Called by the simulation framework to set up the RNG at the start
     * of a simulation run. Seed selection should take place here.
     */
    virtual void initialize(int runnumber, int id, cConfiguration *cfg) = 0;

    /**
     * Random integer in the range [0,intRandMax()]
     */
    virtual unsigned long intRand() = 0;

    /**
     * Maximum value that can be returned by intRand(), e.g. 2^31-2 with LCG32.
     */
    virtual unsigned long intRandMax() = 0;

    /**
     * Random integer in [0,n], n < intRandMax()
     */
    virtual unsigned long intRand(unsigned long n) = 0;

    /**
     * Random double on the [0,1) interval
     */
    virtual double doubleRand() = 0;

    /**
     * Random double on the (0,1) interval
     */
    virtual double doubleRandNonz() = 0;

    /**
     * Random double on the [0,1] interval
     */
    virtual double doubleRandIncl1() = 0;

    /**
     * Random double on the (0,1] interval
     */
    double doubleRandNonzIncl1() {return 1-doubleRand();}
};

#endif

