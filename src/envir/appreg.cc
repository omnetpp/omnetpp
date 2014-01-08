//==========================================================================
//  APPREG.CC - part of
//                     OMNeT++/OMNEST
//            Discrete System Simulation in C++
//
//  Author: Andras Varga
//
//==========================================================================

/*--------------------------------------------------------------*
  Copyright (C) 1992-2008 Andras Varga
  Copyright (C) 2006-2008 OpenSim Ltd.

  This file is distributed WITHOUT ANY WARRANTY. See the file
  `license' for details on this and other legal matters.
*--------------------------------------------------------------*/

#include "appreg.h"

NAMESPACE_BEGIN

cGlobalRegistrationList omnetapps("omnetapps");

EXECUTE_ON_SHUTDOWN( omnetapps.clear() );

cOmnetAppRegistration *cOmnetAppRegistration::chooseBest()
{
    // choose the one with highest score
    cOmnetAppRegistration *best = NULL;
    cRegistrationList *list = omnetapps.getInstance();
    for (int i=0; i<list->size(); i++)
    {
        cOmnetAppRegistration *appreg = static_cast<cOmnetAppRegistration *>(list->get(i));
        if (!best || appreg->getScore() > best->getScore())
            best = appreg;
    }
    return best;
}

NAMESPACE_END

