//==========================================================================
//  AKOUTVECTORMGR.CC - part of
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

#ifdef WITH_AKAROA

#include <akaroa/ak_message.H>
#include <cassert>
#include <cstring>
#include <akaroa.H>
#include "omnetpp/cenvir.h"
#include "omnetpp/csimulation.h"
#include "omnetpp/cmodule.h"
#include "omnetpp/cstatistic.h"
#include "omnetpp/regmacros.h"
#include "omnetpp/cconfiguration.h"
#include "omnetpp/cconfigoption.h"
#include "envirbase.h"
#include "akoutvectormgr.h"

namespace omnetpp {
namespace envir {

Register_Class(AkOutputVectorManager);

Register_PerObjectConfigOption(CFGID_WITH_AKAROA, "with-akaroa", KIND_VECTOR, CFG_BOOL, "false", "Whether the output vector should be under Akaroa control.");

AkOutputVectorManager::AkOutputVectorManager()
{
    ak_declared = false;
    ak_count = 0;
}

AkOutputVectorManager::~AkOutputVectorManager()
{
}

void *AkOutputVectorManager::registerVector(const char *modulename, const char *vectorname)
{
    AkVectorData *vp = (AkVectorData *)OmnetppOutputVectorManager::registerVector(modulename, vectorname);

    // see if this vector needs Akaroa control
    std::string objectfullpath = std::string(modulename) + "." + vectorname;
    vp->ak_controlled = getEnvir()->getConfig()->getAsBool(objectfullpath.c_str(), CFGID_WITH_AKAROA);

    if (vp->ak_controlled) {
        // register vector with Akaroa
        if (ak_declared)
            throw cRuntimeError("cAkOutputVectorManager: With Akaroa, cannot create new vector after first data have been recorded");

        // Akaroa starts parameter numbering with one
        vp->ak_id = ++ak_count;
        AkMessage((char *)"Parameter %i is equivalent to vector: %s.\"%s\".", vp->ak_id, modulename, vectorname);
    }
    return vp;
}

OmnetppOutputVectorManager::VectorData *AkOutputVectorManager::createVectorData()
{
    return new AkVectorData;
}

bool AkOutputVectorManager::record(void *vectorhandle, simtime_t t, double value)
{
    AkVectorData *vp = (AkVectorData *)vectorhandle;
    if (vp->ak_controlled) {
        // 1. register the parameters to Akaroa
        if (!ak_declared) {
            AkDeclareParameters(ak_count);
            AkMessage((char *)"Parameters declared:  %i", ak_count);
            ak_declared = true;
        }

        // 2. transmit value to Akaroa
        AkObservation(vp->ak_id, value);
    }

    // 3. write the vector file too
    return OmnetppOutputVectorManager::record(vectorhandle, t, value);
}
}  // namespace envir
}  // namespace omnetpp

#endif

