//==========================================================================
//  CMERSENNETWISTER.CC - part of
//                         OMNeT++
//              Discrete System Simulation in C++
//
// Contents:
//   class cMersenneTwister
//
//==========================================================================

/*--------------------------------------------------------------*
  Copyright (C) 2002-2004 Andras Varga

  This file is distributed WITHOUT ANY WARRANTY. See the file
  `license' for details on this and other legal matters.
*--------------------------------------------------------------*/

#include "cenvir.h"
#include "util.h"
#include "cexception.h"
#include "cmersennetwister.h"


Register_Class(cMersenneTwister);


void cMersenneTwister::initialize(int runNumber, int id, int numRngs, cConfiguration *cfg)
{
    char section[16], entry[32];
    sprintf(section, "Run %d", runNumber);
    sprintf(entry, "seed-%d-mt", id);

    unsigned long seed = cfg->getAsInt2(section, "General", entry);
    if (cfg->notFound())
    {
        // use the following number as seed, and hope that all seed values
        // are well apart in the 2^19937-long sequence (it should hold if
        // someone did the work of testing the initialization routine).
        seed = runNumber*numRngs + id;
    }
    rng.seed(seed);
}

void cMersenneTwister::selfTest()
{
    rng.seed(1);
    for (int i=0; i<10000; i++)
        intRand();
    if (intRand()!=2915232614UL)
        throw new cException("cMersenneTwister: selfTest() failed, please report this problem!");
}

unsigned long cMersenneTwister::intRand()
{
    return rng.randInt();
}

unsigned long cMersenneTwister::intRandMax()
{
    return 0xffffffffUL; // 2^32-1
}

unsigned long cMersenneTwister::intRand(unsigned long n)
{
    return rng.randInt(n-1);
}

double cMersenneTwister::doubleRand()
{
    return rng.randExc();
}

double cMersenneTwister::doubleRandNonz()
{
    return rng.randDblExc();
}

double cMersenneTwister::doubleRandIncl1()
{
    return rng.rand();
}


