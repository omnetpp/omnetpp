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

#include <typeinfo>
#include <iostream>
#include "cenvir.h"
#include "defs.h"
#include "util.h"
#include "cpolymorphic.h"
#include "cexception.h"

/**
 * Interface for random number generator classes
 */
class cRNG : public cPolymorphic
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
     * Should be redefined to generate a random number on the interval [0,0x7fffffff]
     */
    virtual unsigned long intRand() = 0;

    /**
     * Should be redefined to generate a random number on the [0,1) real interval
     */
    virtual double doubleRand() = 0;
};

#endif

