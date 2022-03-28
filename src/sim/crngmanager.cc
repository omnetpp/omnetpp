//==========================================================================
//   CRNGMANAGER.CC  - part of
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

#include "omnetpp/cconfiguration.h"
#include "omnetpp/cconfigoption.h"
#include "omnetpp/crngmanager.h"
#include "omnetpp/crng.h"
#include "omnetpp/cenvir.h"
#include "omnetpp/csimulation.h"
#include "omnetpp/regmacros.h"
#include "omnetpp/globals.h"
#include "omnetpp/cdynamicexpression.h"
#include "omnetpp/chasher.h"

namespace omnetpp {

Register_PerRunConfigOption(CFGID_NUM_RNGS, "num-rngs", CFG_INT, "1", "The number of random number generators.");
Register_PerRunConfigOption(CFGID_RNG_CLASS, "rng-class", CFG_STRING, "omnetpp::cMersenneTwister", "The random number generator class to be used. It can be `cMersenneTwister`, `cLCG32`, `cAkaroaRNG`, or you can use your own RNG class (it must be subclassed from `cRNG`).");
Register_PerRunConfigOption(CFGID_SEED_SET, "seed-set", CFG_INT, "${runnumber}", "Selects the kth set of automatic random number seeds for the simulation. Meaningful values include `${repetition}` which is the repeat loop counter (see `repeat` option), and `${runnumber}`.");

cIRngManager::~cIRngManager()
{
}

cRngManager::~cRngManager()
{
    for (int i = 0; i < numRNGs; i++)
        delete rngs[i];
    delete[] rngs;
}

void cRngManager::configure(cConfiguration *cfg)
{
    this->cfg = cfg;
    cEnvir *envir = getEnvir(); //TODO eliminate -- pass in parsimProcId and ParsimNumPartitions directly?

    // run RNG self-test on RNG class selected for this run
    std::string rngClass = cfg->getAsString(CFGID_RNG_CLASS);
    cRNG *testRng = createByClassName<cRNG>(rngClass.c_str(), "random number generator");
    testRng->selfTest();
    delete testRng;

    // set up RNGs
    for (int i = 0; i < numRNGs; i++)
        delete rngs[i];
    delete[] rngs;

    numRNGs = cfg->getAsInt(CFGID_NUM_RNGS);
    int seedset = cfg->getAsInt(CFGID_SEED_SET);
    rngs = new cRNG *[numRNGs];
    for (int i = 0; i < numRNGs; i++) {
        rngs[i] = createByClassName<cRNG>(rngClass.c_str(), "random number generator");
        rngs[i]->configure(seedset, i, numRNGs, envir->getParsimProcId(), envir->getParsimNumPartitions(), cfg);
    }
}

void cRngManager::configureRNGs(cComponent *component)
{
    std::string componentFullPath = component->getFullPath();
    std::vector<const char *> suffixes = cfg->getMatchingPerObjectConfigKeySuffixes(componentFullPath.c_str(), "rng-*");  // CFGID_RNG_K
    if (suffixes.empty())
        return;

    // extract into tmpmap[]
    int mapsize = 0;
    int tmpmap[100];
    for (auto suffix : suffixes) {
        // contains "rng-1", "rng-4" or whichever has been found in the config for this module/channel
        const cConfiguration::KeyValue& entry = cfg->getPerObjectConfigEntry(componentFullPath.c_str(), suffix);
        const char *value = entry.getValue();
        ASSERT(value != nullptr);
        try {
            char *endptr;
            int modRng = strtol(suffix+strlen("rng-"), &endptr, 10);
            if (*endptr != '\0')
                throw cRuntimeError("Numeric RNG index expected after 'rng-'");

            int physRng = strtol(value, &endptr, 10);
            if (*endptr != '\0') {
                // not a numeric constant, try parsing it as an expression
                cDynamicExpression expr;
                expr.parse(value);
                cExpression::Context context(component, entry.getBaseDirectory());
                cValue tmp = expr.evaluate(&context);
                if (!tmp.isNumeric())
                    throw cRuntimeError("Numeric constant or expression expected");
                physRng = tmp;
            }

            if (physRng >= getNumRNGs())
                throw cRuntimeError("RNG index %d out of range (num-rngs=%d)", physRng, getNumRNGs());
            if (modRng >= mapsize) {
                if (modRng >= 100)
                    throw cRuntimeError("Local RNG index %d out of supported range 0..99", modRng);
                while (mapsize <= modRng) {
                    tmpmap[mapsize] = mapsize;
                    mapsize++;
                }
            }
            tmpmap[modRng] = physRng;
        }
        catch (std::exception& e) {
            throw cRuntimeError("%s in configuration entry *.%s = %s for module/channel %s",
                    e.what(), suffix, value, component->getFullPath().c_str());

        }
    }

    // install map into the module
    if (mapsize > 0) {
        int *map = new int[mapsize];
        memcpy(map, tmpmap, mapsize*sizeof(int));
        component->setRNGMap(mapsize, map);
    }
}

int cRngManager::getNumRNGs(const cComponent *component) const
{
    return numRNGs;
}

cRNG *cRngManager::getRNG(const cComponent *component, int k)
{
    int rngId = k < component->rngMapSize ? component->rngMap[k] : k;
    return getRNG(rngId);
}

int cRngManager::getNumRNGs() const
{
    return numRNGs;
}

cRNG *cRngManager::getRNG(int rngId)
{
    if (rngId < 0 || rngId >= numRNGs)
        throw cRuntimeError("RNG index %d is out of range (num-rngs=%d, check the configuration)", rngId, numRNGs);
    return rngs[rngId];
}

uint32_t cRngManager::getHash() const
{
    cHasher hasher;
    for (int i = 0; i < numRNGs; i++)
        hasher << rngs[i]->getNumbersDrawn();
    return hasher.getHash();
}

}  // namespace omnetpp

