//==========================================================================
//  LCGRANDOM.H - part of
//                     OMNeT++/OMNEST
//            Discrete System Simulation in C++
//
//  Author: Andras Varga
//
//==========================================================================

/*--------------------------------------------------------------*
  Copyright (C) 2006-2015 OpenSim Ltd.

  This file is distributed WITHOUT ANY WARRANTY. See the file
  `license' for details on this and other legal matters.
*--------------------------------------------------------------*/

#ifndef __OMNETPP_LCGRANDOM_H
#define __OMNETPP_LCGRANDOM_H

#include <stdint.h>
#include "commondefs.h"
#include "exception.h"

NAMESPACE_BEGIN

#define GLRAND_MAX  0x7ffffffeL  /* = 2**31-2 */

/**
 * Our RNG on [0,1), for the layouting library and other non-simulation purposes.
 * C's rand() is not to be trusted.
 */
class COMMON_API LCGRandom
{
    private:
        int32_t seed;

    public:
        LCGRandom(int32_t seed=1);

        int32_t getSeed() { return seed; }
        void setSeed(int32_t seed);

        double next01();

        int draw(int range);

        void selfTest();
};

NAMESPACE_END


#endif
