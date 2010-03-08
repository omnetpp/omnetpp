//==========================================================================
//  RESULTLISTENER.CC - part of
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

#include "resultlistener.h"
#include "resultfilters.h"
#include "ccomponent.h"

void SignalSource::subscribe(ResultListener *listener) const
{
    if (filter)
        filter->addDelegate(listener);
    else if (component && signalID!=SIMSIGNAL_NULL)
        component->subscribe(signalID, listener);
    else
        throw opp_runtime_error("subscribe() called on blank SignalSource");
}

