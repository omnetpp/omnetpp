//==========================================================================
//  INSPECTORFACTORY.CC - part of
//
//                     OMNeT++/OMNEST
//            Discrete System Simulation in C++
//
//==========================================================================

/*--------------------------------------------------------------*
  Copyright (C) 1992-2008 Andras Varga
  Copyright (C) 2006-2008 OpenSim Ltd.

  This file is distributed WITHOUT ANY WARRANTY. See the file
  `license' for details on this and other legal matters.
*--------------------------------------------------------------*/

#include <stdarg.h>
#include "omnetpp.h"
#include "tkenv.h"
#include "tklib.h"
#include "inspector.h"
#include "inspectorfactory.h"

NAMESPACE_BEGIN

cGlobalRegistrationList inspectorfactories;

EXECUTE_ON_SHUTDOWN( inspectorfactories.clear() );


InspectorFactory *findInspectorFactoryFor(cObject *obj, int type)
{
    InspectorFactory *best=NULL;
    double bestweight=0;
    cRegistrationList *a = inspectorfactories.getInstance();
    for (int i=0; i<a->size(); i++)
    {
        InspectorFactory *ifc = static_cast<InspectorFactory *>(a->get(i));
        if (ifc->supportsObject(obj) &&
            (type==INSP_DEFAULT || ifc->getInspectorType()==type) &&
            ifc->getQualityAsDefault(obj)>bestweight
           )
        {
            bestweight = ifc->getQualityAsDefault(obj);
            best = ifc;
        }
    }
    return best; // may be NULL too
}

InspectorFactory *InspectorFactory::find(const char *className)
{
    return dynamic_cast<InspectorFactory *>(inspectorfactories.getInstance()->find(className));
}

InspectorFactory *InspectorFactory::get(const char *className)
{
    InspectorFactory *p = find(className);
    if (!p)
        throw cRuntimeError("Inspectorfactory \"%s\" not found", className);
    return p;
}


NAMESPACE_END
