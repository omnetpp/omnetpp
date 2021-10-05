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
  Copyright (C) 2002-2017 Andras Varga
  Copyright (C) 2006-2017 OpenSim Ltd.

  This file is distributed WITHOUT ANY WARRANTY. See the file
  `license' for details on this and other legal matters.
*--------------------------------------------------------------*/

#include "omnetpp/clog.h"
#include "omnetpp/cenvir.h"
#include "omnetpp/simutil.h"
#include "omnetpp/cexception.h"
#include "omnetpp/cmersennetwister.h"
#include "omnetpp/cmessage.h"
#include "omnetpp/cconfigoption.h"

namespace omnetpp {

Register_Class(cMersenneTwister);

Register_PerRunConfigOption(CFGID_SEED_N_MT, "seed-%-mt", CFG_INT, nullptr, "When Mersenne Twister is selected as random number generator (default): seed for RNG number k. (Substitute k for '%' in the key.)");
Register_PerRunConfigOption(CFGID_SEED_N_MT_P, "seed-%-mt-p%", CFG_INT, nullptr, "With parallel simulation: When Mersenne Twister is selected as random number generator (default): seed for RNG number k in partition number p. (Substitute k for the first '%' in the key, and p for the second.)");

void cMersenneTwister::initialize(int seedSet, int rngId, int numRngs,
        int parsimProcId, int parsimNumPartitions,
        cConfiguration *cfg)
{
    char key[40], key2[40];
    sprintf(key, "seed-%d-mt", rngId);
    sprintf(key2, "seed-%d-mt-p%d", rngId, parsimProcId);

    uint32_t seed;
    if (parsimNumPartitions > 1) {
        // with parallel simulation, every partition should get distinct streams
        const char *value = cfg->getConfigValue(key2);
        if (value != nullptr) {
            seed = cConfiguration::parseLong(value, nullptr);
        }
        else {
            if (cfg->getConfigValue(key) != nullptr)
                EV << "Warning: cMersenneTwister: ignoring config key " << key << "=<seed>"
                   << " for parallel simulation -- please use partition-specific variant "
                   << key2 << "=<seed>\n";
            seed = (seedSet * numRngs + rngId) * MAX_PARSIM_PARTITIONS + parsimProcId;
        }
    }
    else {
        const char *value = cfg->getConfigValue(key);
        if (value != nullptr)
            seed = cConfiguration::parseLong(value, nullptr);
        else
            seed = seedSet * numRngs + rngId;
    }

    // use the following number as seed, and hope that all seed values
    // are well apart in the 2^19937-long sequence (it should hold if
    // someone did the work of testing the initialization routine).
    rng.seed(seed);
}

void cMersenneTwister::selfTest()
{
    rng.seed(1);
    for (int i = 0; i < 10000; i++)
        intRand();
    if (intRand() != 2915232614UL)
        throw cRuntimeError("cMersenneTwister: selfTest() failed, please report this problem!");
}

uint32_t cMersenneTwister::intRand()
{
    numDrawn++;
    return rng.randInt();
}

uint32_t cMersenneTwister::intRandMax()
{
    return 0xffffffffUL;  // 2^32-1
}

uint32_t cMersenneTwister::intRand(uint32_t n)
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

}  // namespace omnetpp

