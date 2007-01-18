//==========================================================================
//  RANDOM.CC - part of
//                     OMNeT++/OMNEST
//            Discrete System Simulation in C++
//
//==========================================================================

/*--------------------------------------------------------------*
  Copyright (C) 1992-2005 Andras Varga

  This file is distributed WITHOUT ANY WARRANTY. See the file
  `license' for details on this and other legal matters.
*--------------------------------------------------------------*/

#include "lcgrandom.h"

LCGRandom::LCGRandom(int32 seed)
{
    setSeed(seed);
}

void LCGRandom::setSeed(int32 seed)
{
    this->seed = seed;

    // consume some values, so that small seeds will work correctly
    next01();
    next01();
    next01();
}

double LCGRandom::next01()
{
    const long int a = 16807, q = 127773, r = 2836;
    seed = a * (seed % q) - r * (seed / q);
    if (seed <= 0) seed += GLRAND_MAX + 1;

    return seed / (double)(GLRAND_MAX + 1);
}
