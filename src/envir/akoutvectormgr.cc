//==========================================================================
//   AKOUTVECTORMGR.CC - header for
//                             OMNeT++
//            Discrete System Simulation in C++
//
//  Members of the following classes:
//     AkOutputVectorManager
//     AkOutputScalarManager
//     AkSnapshotManager
//
//==========================================================================

/*--------------------------------------------------------------*
  Copyright (C) 1992-2002 Andras Varga
  Technical University of Budapest, Dept. of Telecommunications,
  Stoczek u.2, H-1111 Budapest, Hungary.

  This file is distributed WITHOUT ANY WARRANTY. See the file
  `license' for details on this and other legal matters.
*--------------------------------------------------------------*/

#ifdef WITH_AKAROA

#include <assert.h>
#include <string.h>
#include <fstream.h>   // ostream
#include <iostream.h>  // ostream
#include "cenvir.h"
#include "omnetapp.h"
#include "cinifile.h"
#include "csimul.h"
#include "cmodule.h"
#include "cstat.h"
#include "macros.h"
#include "akfilemgrs.h"
#include <akaroa.H>
#include <akaroa/ak_message.H>



Register_Class(AkOutputVectorManager);


AkOutputVectorManager::AkOutputVectorManager()
{
    ak_declared = false;
    ak_count = 0;
}

AkOutputVectorManager::~AkOutputVectorManager()
{
}

void *AkOutputVectorManager::registerVector(const char *modulename, const char *vectorname, int tuple)
{
    sAkVectorData *vp = (sAkVectorData *)cFileOutputVectorManager::registerVector(modulename,vectorname, tuple);

    // see if this vector needs Akaroa control
    opp_string inientry;
    inientry.allocate(opp_strlen(modulename)+1+opp_strlen(vectorname)+sizeof(".akaroa")+1);
    sprintf(inientry.buffer(),"%s.%s.akaroa", modulename, vectorname);
    vp->ak_controlled = ev.app->getIniFile()->getAsBool2(section, "General", (const char *)inientry, true);

    if (vp->ak_controlled)
    {
        // register vector with Akaroa
        if (ak_registered)
            opp_error("AkOutputVectorManager: With Akaroa, cannot create new vector after first data have been recorded");

        if (tuple > 1)
            opp_error("AkOutputVectorManager: Akaroa can handle only one parameter per cOutVector");

        // Akaroa starts parameter numbering with one
        vp->ak_id = ++ak_count;
        AkMessage("Parameter %i is equivalent to vector: %s.\"%s\".", vp->ak_id, modulename, vectorname);
    }
    return vp;
}

sVectorData *cAkOutputVectorManager::createVectorData()
{
    return new sAkVectorData;
}

bool AkOutputVectorManager::record(void *vectorhandle, simtime_t t, double value)
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
    return cFileOutputManager::record(vectorhandle, t, value);
}

bool AkOutputVectorManager::record(void *vectorhandle, simtime_t t, double value1, double value2)
{
    opp_error("AkOutputVectorManager: Akaroa can handle only one parameter per cOutVector");
    return false;
}

#endif


