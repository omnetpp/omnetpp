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

class cConfiguration;


/**
 * Interface for random number generator classes.
 * 
 * See also cModule::rng().
 *
 * @ingroup RandomNumbers
 * @ingroup EnvirExtensions
 */
class SIM_API cRNG : public cPolymorphic
{
  protected:
    unsigned long numDrawn;

  public:
    cRNG() {numDrawn=0;}
    virtual ~cRNG() {}

    /**
     * Called by the simulation framework to set up the RNG at the start
     * of a simulation run. Seed selection should take place here.
     */
    virtual void initialize(int runNumber, int id, int numRngs, cConfiguration *cfg) = 0;

    /**
     * Coarse test for the correctness of the RNG algorithm. It should detect
     * platform-dependent bugs (e.g. caused by different word size or compiler
     * anomaly). The method should throw an exception if something is wrong.
     * It is invoked only once, when the simulation program starts up.
     */
    virtual void selfTest() = 0;

    /**
     * Returns how many random numbers have been drawn from this RNG.
     * Subclasses should increment numDrawn in the intRand(), etc. methods.
     */
    virtual unsigned long numbersDrawn() {return numDrawn;}

    /**
     * Random integer in the range [0,intRandMax()]
     */
    virtual unsigned long intRand() = 0;

    /**
     * Maximum value that can be returned by intRand(), e.g. 2^31-2 with LCG32.
     */
    virtual unsigned long intRandMax() = 0;

    /**
     * Random integer in [0,n), n < intRandMax()
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

