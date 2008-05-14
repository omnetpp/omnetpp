//==========================================================================
//  AKAROARNG.CC - part of
//                 OMNeT++/OMNEST
//              Discrete System Simulation in C++
//
// Contents:
//   class cAkaroaRNG
//
//  Author: Andras Varga
//
//==========================================================================

/*--------------------------------------------------------------*
  Copyright (C) 2002-2008 Andras Varga
  Copyright (C) 2006-2008 OpenSim Ltd.

  This file is distributed WITHOUT ANY WARRANTY. See the file
  `license' for details on this and other legal matters.
*--------------------------------------------------------------*/

#ifdef WITH_AKAROA

#include <akaroa.H>
#include <akaroa/distributions.H>  // for UniformInt()
#include "cenvir.h"
#include "util.h"
#include "cexception.h"
#include "akaroarng.h"

USING_NAMESPACE


Register_Class(cAkaroaRNG);


void cAkaroaRNG::initialize(int runNumber, int id, int numRngs, cConfiguration *cfg)
{
    // no manual seeding, and only one RNG stream
    if (numRngs!=1 || id!=0)
        throw cRuntimeError("cAkaroaRNG: Akaroa supports only one RNG stream, use num-rngs=1 setting");
}

void cAkaroaRNG::selfTest()
{
    // Akaroa doesn't provide self-test capability
}

unsigned long cAkaroaRNG::intRand()
{
    numDrawn++;
    return AkRandom()-1;
}

unsigned long cAkaroaRNG::intRandMax()
{
    return 0x7ffffffeUL; // 2^31-2
}

unsigned long cAkaroaRNG::intRand(unsigned long n)
{
    // UniformInt() is also from Akaroa, and it's based on AkRandomReal()
    numDrawn++;
    return UniformInt(0, n-1);
}

double cAkaroaRNG::doubleRand()
{
    // khmm, this cannot return zero, but we cannot do better... see class doc.
    numDrawn++;
    return AkRandomReal();
}

double cAkaroaRNG::doubleRandNonz()
{
    numDrawn++;
    return AkRandomReal();
}

double cAkaroaRNG::doubleRandIncl1()
{
    // khmm, this cannot return 1, but we cannot do better... see class doc.
    numDrawn++;
    return AkRandomReal();
}

#endif

