//==========================================================================
//   SIMTIMEMATH.H  - part of
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

#include "omnetpp/simtimemath.h"
#include "omnetpp/cexception.h"

namespace omnetpp {

static int64_t gcd(int64_t a, int64_t b)
{
    if (b == 0)
        return a;
    else
        return gcd(b, a % b);
}

int64_t preciseDiv(int64_t x, const SimTime& y, int64_t& fractionNumerator, int64_t& fractionDenominator)
{
    int64_t raw = y.raw();
    int64_t scale = y.getScale();

    if (raw == 0)
        throw cRuntimeError("previseDiv(): division by zero");

    int64_t d = gcd(scale, raw);
    if (d > 1 || d < -1) {
        scale /= d;
        raw /= d;
    }

    d = gcd(x, raw);
    if (d > 1 || d < -1) {
        x /= d;
        raw /= d;
    }

    int64_t p = x * scale;
    if (p / scale != x)
        throw cRuntimeError("previseDiv(): overflow of intermediate result");
    if (raw == 1) {
        fractionNumerator = 0;
        fractionDenominator = 1;
        return p;

    }

    int64_t result = p / raw;
    fractionNumerator = p - result*raw;
    fractionDenominator = raw;

    if (fractionDenominator < 0) {
        fractionNumerator = -fractionNumerator;
        fractionDenominator = -fractionDenominator;
    }
    return result;
}

}  // namespace omnetpp


