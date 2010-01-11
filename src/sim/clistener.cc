//=========================================================================
//  CLISTENER.CC - part of
//
//                  OMNeT++/OMNEST
//           Discrete System Simulation in C++
//
//=========================================================================

/*--------------------------------------------------------------*
  Copyright (C) 1992-2008 Andras Varga
  Copyright (C) 2006-2008 OpenSim Ltd.

  This file is distributed WITHOUT ANY WARRANTY. See the file
  `license' for details on this and other legal matters.
*--------------------------------------------------------------*/

#include "clistener.h"
#include "ccomponent.h"

USING_NAMESPACE

void cListener::unsupportedType(simsignal_t signalID, const char *dataType)
{
    const char *signalName = cComponent::getSignalName(signalID);
    throw cRuntimeError("%s: Unsupported signal data type %s for signal %s (id=%d)",
                        opp_typename(typeid(*this)), dataType, signalName, (int)signalID);
}

void cListener::receiveSignal(cComponent *source, simsignal_t signalID, long l)
{
    unsupportedType(signalID, "long");
}

void cListener::receiveSignal(cComponent *source, simsignal_t signalID, double d)
{
    unsupportedType(signalID, "double");
}

void cListener::receiveSignal(cComponent *source, simsignal_t signalID, simtime_t t)
{
    unsupportedType(signalID, "simtime_t");
}

void cListener::receiveSignal(cComponent *source, simsignal_t signalID, const char *s)
{
    unsupportedType(signalID, "const char *");
}

void cListener::receiveSignal(cComponent *source, simsignal_t signalID, cObject *obj)
{
    unsupportedType(signalID, "cObject *");
}

