//==========================================================================
//   CRNG.H  - part of
//                     OMNeT++/OMNEST
//            Discrete System Simulation in C++
//
//==========================================================================

/*--------------------------------------------------------------*
  Copyright (C) 1992-2017 Andras Varga
  Copyright (C) 2006-2017 OpenSim Ltd.

  This file is distributed WITHOUT ANY WARRANTY. See the file
  `license' for details on this and other legal matters.
*--------------------------------------------------------------*/

#ifndef __OMNETPP_CRNG_H
#define __OMNETPP_CRNG_H

#include "simkerneldefs.h"
#include "cobject.h"

namespace omnetpp {

class cConfiguration;


/**
 * @brief Abstract interface for random number generator classes.
 *
 * Some known implementations are <tt>cMersenneTwister</tt>,
 * <tt>cLCG32</tt> and <tt>cAkaroaRNG</tt>. The actual RNG class
 * to be used in simulations can be configured (a feature of the
 * Envir library).
 *
 * To switch to your own RNG, subclass cRNG, register your new class
 * with the Register_Class() macro, then select it
 * by adding the following to <tt>omnetpp.ini</tt>:
 *
 * <pre>
 * [General]
 * rng-class="MyClass"
 * </pre>
 *
 * See also cModule::getRNG().
 *
 * @ingroup RandomNumbers
 */
class SIM_API cRNG : public cObject
{
  protected:
    uint64_t numDrawn = 0;

  public:
    cRNG() {}
    virtual ~cRNG() {}

    /**
     * Called by the simulation framework to set up the RNG at the start
     * of a simulation run. Seed selection should take place here.
     */
    virtual void initialize(int seedSet, int rngId, int numRngs,
                            int parsimProcId, int parsimNumPartitions,
                            cConfiguration *cfg) = 0;

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
    virtual uint64_t getNumbersDrawn() const  {return numDrawn;}

    /**
     * Random integer in the range [0,intRandMax()]
     */
    virtual uint32_t intRand() = 0;

    /**
     * Maximum value that can be returned by intRand(), e.g. 2^31-2 with LCG32.
     */
    virtual uint32_t intRandMax() = 0;

    /**
     * Random integer in [0,n), n < intRandMax()
     */
    virtual uint32_t intRand(uint32_t n) = 0;

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

}  // namespace omnetpp


#endif

