//==========================================================================
//  RANDOM.H - part of
//                     OMNeT++/OMNEST
//            Discrete System Simulation in C++
//
//
//  Declaration of the following classes:
//    StringTokenizer  : string tokenizer utility class
//
//==========================================================================

/*--------------------------------------------------------------*
  Copyright (C) 1992-2005 Andras Varga

  This file is distributed WITHOUT ANY WARRANTY. See the file
  `license' for details on this and other legal matters.
*--------------------------------------------------------------*/

#include "inttypes.h"

#ifndef __COMMON_RANDOM_H
#define __COMMON_RANDOM_H

#define GLRAND_MAX  0x7ffffffeL  /* = 2**31-2 */

// our RNG on [0,1) -- C's rand() is not to be trusted
class Random
{
    private:
        int32 seed;

    public:
        Random(int32 seed = 1);

        void setSeed(int32 seed) { this->seed = seed; }

        double next01();
};

#endif
