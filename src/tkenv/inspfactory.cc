//==========================================================================
//   INSPECT.CC -
//            part of the Tcl/Tk environment of
//                             OMNeT++
//
//  Implementation of inspector() functions
//
//==========================================================================

/*--------------------------------------------------------------*
  Copyright (C) 1992-2001 Andras Varga
  Technical University of Budapest, Dept. of Telecommunications,
  Stoczek u.2, H-1111 Budapest, Hungary.

  This file is distributed WITHOUT ANY WARRANTY. See the file
  `license' for details on this and other legal matters.
*--------------------------------------------------------------*/

#include <stdarg.h>
#include "omnetpp.h"
#include "tkapp.h"
#include "tklib.h"
#include "inspector.h"
#include "inspfactory.h"


cHead inspectorfactories("inspectorfactories");


cInspectorFactory *findInspectorFactoryFor(cObject *obj, int type)
{
    if (type!=INSP_DEFAULT)
    {
        // choose the 1st one that can create this type
        // FIXME: not good, see cPacket vs cMessage, INSP_OBJECT!!!
        for (cIterator i(inspectorfactories); !i.end(); i++)
        {
            cInspectorFactory *ifc = (cInspectorFactory *) i();
            if (ifc->supportsObject(obj) && ifc->inspectorType()==type)
                return ifc;
        }
        return NULL;
    }
    else
    {
        // otherwise choose best one
        cInspectorFactory *best=NULL;
        double bestweight=0;
        for (cIterator i(inspectorfactories); !i.end(); i++)
        {
            cInspectorFactory *ifc = (cInspectorFactory *) i();
            if (ifc->supportsObject(obj) && ifc->qualityAsDefault(obj)>bestweight)
            {
                bestweight = ifc->qualityAsDefault(obj);
                best = ifc;
            }
        }
        return best; // may be NULL too
    }
}


