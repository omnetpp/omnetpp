//==========================================================================
//  AKAROARNG.CC - part of
//                         OMNeT++
//              Discrete System Simulation in C++
//
// Contents:
//   class cAkaroaRNG
//
//==========================================================================

/*--------------------------------------------------------------*
  Copyright (C) 2002-2004 Andras Varga

  This file is distributed WITHOUT ANY WARRANTY. See the file
  `license' for details on this and other legal matters.
*--------------------------------------------------------------*/

#ifdef WITH_AKAROA

#include <akaroa.H>
#include "cenvir.h"
#include "util.h"
#include "cexception.h"
#include "akaroarng.h"


Register_Class(cAkaroaRNG);


void cAkaroaRNG::initialize(int runNumber, int id, int numRngs, cConfiguration *cfg)
{
    // no manual seeding, and only one RNG stream
    if (numRngs!=1 || id!=0)
        throw new cException("cAkaroaRNG: Akaroa supports only one RNG stream, use num-rngs=1 setting");
}

void cAkaroaRNG::selfTest()
{
    // Akaroa doesn't provide self-test capability
}

unsigned long cAkaroaRNG::intRand()
{
    return AkRandom()-1;
}

unsigned long cAkaroaRNG::intRandMax()
{
    return 0x7ffffffeUL; // 2^31-2
}

unsigned long cAkaroaRNG::intRand(unsigned long n)
{
    // UniformInt() is also from Akaroa, and it's based on AkRandomReal()
    return UniformInt(0, n-1);
}

double cAkaroaRNG::doubleRand()
{
    // khmm, can't do better... see class doc.
    return AkRandomReal();
}

double cAkaroaRNG::doubleRandNonz()
{
    return AkRandomReal();
}

double cAkaroaRNG::doubleRandIncl1()
{
    // khmm, can't do better... see class doc.
    return AkRandomReal();
}

#endif

