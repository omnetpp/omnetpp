//==========================================================================
//  AKOUTVECTORMGR.CC - part of
//                     OMNeT++/OMNEST
//            Discrete System Simulation in C++
//
//  Members of the following classes:
//     cAkOutputVectorManager
//     AkOutputScalarManager
//     AkSnapshotManager
//
//==========================================================================

/*--------------------------------------------------------------*
  Copyright (C) 1992-2005 Andras Varga

  This file is distributed WITHOUT ANY WARRANTY. See the file
  `license' for details on this and other legal matters.
*--------------------------------------------------------------*/

#ifdef WITH_AKAROA

#include <assert.h>
#include <string.h>
#include <akaroa.H>
#include <akaroa/ak_message.H>
#include "cenvir.h"
#include "omnetapp.h"
#include "csimul.h"
#include "cmodule.h"
#include "cstat.h"
#include "macros.h"
#include "akoutvectormgr.h"

USING_NAMESPACE


Register_Class(cAkOutputVectorManager);


Register_PerObjectConfigEntry(CFGID_WITH_AKAROA, "with-akaroa", CFG_BOOL, "false", "Whether to the output vector should be under Akaroa control.");


cAkOutputVectorManager::cAkOutputVectorManager()
{
    ak_declared = false;
    ak_count = 0;
}

cAkOutputVectorManager::~cAkOutputVectorManager()
{
}

void *cAkOutputVectorManager::registerVector(const char *modulename, const char *vectorname)
{
    sAkVectorData *vp = (sAkVectorData *)cFileOutputVectorManager::registerVector(modulename, vectorname);

    // see if this vector needs Akaroa control
    std::string objectfullpath = std::string(modulename) + "." + vectorname;
    vp->ak_controlled = ev.config()->getAsBool(objectfullpath.c_str(), CFGID_WITH_AKAROA);

    if (vp->ak_controlled)
    {
        // register vector with Akaroa
        if (ak_declared)
            throw cRuntimeError("cAkOutputVectorManager: With Akaroa, cannot create new vector after first data have been recorded");

        // Akaroa starts parameter numbering with one
        vp->ak_id = ++ak_count;
        AkMessage("Parameter %i is equivalent to vector: %s.\"%s\".", vp->ak_id, modulename, vectorname);
    }
    return vp;
}

cFileOutputVectorManager::sVectorData *cAkOutputVectorManager::createVectorData()
{
    return new sAkVectorData;
}

bool cAkOutputVectorManager::record(void *vectorhandle, simtime_t t, double value)
{
    sAkVectorData *vp = (sAkVectorData *)vectorhandle;
    if (vp->ak_controlled)
    {
        // 1. register the parameters to Akaroa
        if (!ak_declared)
        {
            AkDeclareParameters(ak_count);
            AkMessage("Parameters declared:  %i", ak_count);
            ak_declared = true;
        }

        // 2. transmit value to Akaroa
        AkObservation(vp->ak_id, value);
    }

    // 3. write the vector file too
    return cFileOutputVectorManager::record(vectorhandle, t, value);
}

bool cAkOutputVectorManager::record(void *vectorhandle, simtime_t t, double value1, double value2)
{
    throw cRuntimeError("cAkOutputVectorManager: Akaroa can handle only one parameter per cOutVector");
}

#endif


