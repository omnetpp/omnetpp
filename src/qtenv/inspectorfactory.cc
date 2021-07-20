//==========================================================================
//  INSPECTORFACTORY.CC - part of
//
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

#include <cstdarg>
#include "omnetpp/cobject.h"
#include "qtenv.h"
#include "inspector.h"
#include "inspectorfactory.h"

using namespace omnetpp::common;

namespace omnetpp {
namespace qtenv {

cGlobalRegistrationList inspectorfactories;

EXECUTE_ON_SHUTDOWN(inspectorfactories.clear());

InspectorFactory *findInspectorFactoryFor(cObject *obj, InspectorType type)
{
    InspectorFactory *best = nullptr;
    double bestweight = NEGATIVE_INFINITY;
    cRegistrationList *a = inspectorfactories.getInstance();
    for (int i = 0; i < a->size(); i++) {
        InspectorFactory *ifc = static_cast<InspectorFactory *>(a->get(i));
        if (ifc->supportsObject(obj) &&
            (type == INSP_DEFAULT || ifc->getInspectorType() == type) &&
            ifc->getQualityAsDefault(obj) > bestweight
            )
        {
            bestweight = ifc->getQualityAsDefault(obj);
            best = ifc;
        }
    }
    return best;  // may be nullptr too
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

}  // namespace qtenv
}  // namespace omnetpp

