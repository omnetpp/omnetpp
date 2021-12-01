//==========================================================================
//  LCGRANDOM.H - part of
//                     OMNeT++/OMNEST
//            Discrete System Simulation in C++
//
//==========================================================================

/*--------------------------------------------------------------*
  Copyright (C) 2006-2017 OpenSim Ltd.

  This file is distributed WITHOUT ANY WARRANTY. See the file
  `license' for details on this and other legal matters.
*--------------------------------------------------------------*/

#ifndef __OMNETPP_COMMON_LCGRANDOM_H
#define __OMNETPP_COMMON_LCGRANDOM_H

#include <cstdint>
#include "commondefs.h"
#include "exception.h"

namespace omnetpp {
namespace common {

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

}  // namespace common
}  // namespace omnetpp


#endif
