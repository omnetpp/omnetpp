//==========================================================================
//  LCGRANDOM.CC - part of
//                     OMNeT++/OMNEST
//            Discrete System Simulation in C++
//
//==========================================================================

/*--------------------------------------------------------------*
  Copyright (C) 2006-2017 OpenSim Ltd.

  This file is distributed WITHOUT ANY WARRANTY. See the file
  `license' for details on this and other legal matters.
*--------------------------------------------------------------*/

#include <cmath>
#include "lcgrandom.h"
#include <cassert>

namespace omnetpp {
namespace common {

LCGRandom::LCGRandom(int32_t seed)
{
    // do a self-test the very first time this class is used
    static bool firstTime = true;
    if (firstTime) {
        firstTime = false;
        selfTest();
    }

    setSeed(seed);
}

void LCGRandom::setSeed(int32_t seed)
{
    if (seed < 1 || seed > GLRAND_MAX)
        throw opp_runtime_error("LCGRandom: Invalid seed: %d", seed);

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
    if (seed <= 0)
        seed += GLRAND_MAX + 1;

    return seed / (double)(GLRAND_MAX + 1);
}

int LCGRandom::draw(int range)
{
    double d = next01();
    return (int)floor(range*d);
}

void LCGRandom::selfTest()
{
    seed = 1;
    for (int i = 0; i < 10000; i++)
        next01();
    if (seed != 1043618065L)
        throw opp_runtime_error("LCGRandom: Self test failed, please report this problem!");
}

}  // namespace common
}  // namespace omnetpp

