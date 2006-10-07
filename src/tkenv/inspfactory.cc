//==========================================================================
//  INSPECT.CC - part of
//
//                     OMNeT++/OMNEST
//            Discrete System Simulation in C++
//
//  Implementation of inspector() functions
//
//==========================================================================

/*--------------------------------------------------------------*
  Copyright (C) 1992-2005 Andras Varga

  This file is distributed WITHOUT ANY WARRANTY. See the file
  `license' for details on this and other legal matters.
*--------------------------------------------------------------*/

#ifdef _MSC_VER
#pragma warning(disable:4786)
#endif

#include <stdarg.h>
#include "omnetpp.h"
#include "tkapp.h"
#include "tklib.h"
#include "inspector.h"
#include "inspfactory.h"


cSingleton<cArray> inspectorfactories("inspectorfactories");


cInspectorFactory *findInspectorFactoryFor(cObject *obj, int type)
{
    cInspectorFactory *best=NULL;
    double bestweight=0;
    for (cArray::Iterator it(*inspectorfactories.instance()); !it.end(); it++)
    {
        cInspectorFactory *ifc = static_cast<cInspectorFactory *>(it());
        if (ifc->supportsObject(obj) &&
            (type==INSP_DEFAULT || ifc->inspectorType()==type) &&
            ifc->qualityAsDefault(obj)>bestweight
           )
        {
            bestweight = ifc->qualityAsDefault(obj);
            best = ifc;
        }
    }
    return best; // may be NULL too
}


