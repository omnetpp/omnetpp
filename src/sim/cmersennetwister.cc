//==========================================================================
//  CMERSENNETWISTER.CC - part of
//                 OMNeT++/OMNEST
//              Discrete System Simulation in C++
//
// Contents:
//   class cMersenneTwister
//
//==========================================================================

/*--------------------------------------------------------------*
  Copyright (C) 2002-2005 Andras Varga

  This file is distributed WITHOUT ANY WARRANTY. See the file
  `license' for details on this and other legal matters.
*--------------------------------------------------------------*/

#include "cenvir.h"
#include "util.h"
#include "cexception.h"
#include "cmersennetwister.h"
#include "cparsimcomm.h"  // MAX_PARSIM_PARTITIONS


Register_Class(cMersenneTwister);


void cMersenneTwister::initialize(int runNumber, int rngId, int numRngs,
                                  int parsimProcId, int parsimNumPartitions,
                                  cConfiguration *cfg)
{
    char section[16], entry[40], entry2[40];
    sprintf(section, "Run %d", runNumber);
    sprintf(entry, "seed-%d-mt", rngId);
    sprintf(entry2, "seed-%d-mt-p%d", rngId, parsimProcId);

    unsigned long seed;
    if (parsimNumPartitions>1)
    {
        // with parallel simulation, every partition should get distinct streams
        seed = cfg->getAsInt2(section, "General", entry2);
        if (cfg->notFound())
        {
            if (cfg->exists2(section, "General", entry))
                ev << "Warning: cMersenneTwister: ignoring config entry " << entry << "=<seed>"
                   << " for parallel simulation -- please use partition-specific variant "
                   << entry2 << "=<seed>\n";
            seed = (runNumber*numRngs + rngId)*MAX_PARSIM_PARTITIONS + parsimProcId;
        }
    }
    else
    {
        seed = cfg->getAsInt2(section, "General", entry);
        if (cfg->notFound())
            seed = runNumber*numRngs + rngId;
    }

    // use the following number as seed, and hope that all seed values
    // are well apart in the 2^19937-long sequence (it should hold if
    // someone did the work of testing the initialization routine).
    rng.seed(seed);
}

void cMersenneTwister::selfTest()
{
    rng.seed(1);
    for (int i=0; i<10000; i++)
        intRand();
    if (intRand()!=2915232614UL)
        throw new cRuntimeError("cMersenneTwister: selfTest() failed, please report this problem!");
}

unsigned long cMersenneTwister::intRand()
{
    numDrawn++;
    return rng.randInt();
}

unsigned long cMersenneTwister::intRandMax()
{
    return 0xffffffffUL; // 2^32-1
}

unsigned long cMersenneTwister::intRand(unsigned long n)
{
    numDrawn++;
    return rng.randInt(n-1);
}

double cMersenneTwister::doubleRand()
{
    numDrawn++;
    return rng.randExc();
}

double cMersenneTwister::doubleRandNonz()
{
    numDrawn++;
    return rng.randDblExc();
}

double cMersenneTwister::doubleRandIncl1()
{
    numDrawn++;
    return rng.rand();
}


