//========================================================================
//  CDELAYCHANNEL.CC - part of
//
//                 OMNeT++/OMNEST
//              Discrete System Simulation in C++
//
//   Member functions of
//    cDelayChannel : channel class
//
//========================================================================

/*--------------------------------------------------------------*
  Copyright (C) 1992-2008 Andras Varga
  Copyright (C) 2006-2008 OpenSim Ltd.

  This file is distributed WITHOUT ANY WARRANTY. See the file
  `license' for details on this and other legal matters.
*--------------------------------------------------------------*/

#include "cdelaychannel.h"
#include "cmessage.h"
#include "cenvir.h"
#include "globals.h"
#include "cgate.h"
#include "cexception.h"

#ifdef WITH_PARSIM
#include "ccommbuffer.h"
#endif

USING_NAMESPACE

using std::ostream;

Register_Class(cDelayChannel);


void cDelayChannel::finalizeParameters()
{
    cChannel::finalizeParameters();
    rereadPars();
}

void cDelayChannel::rereadPars()
{
    delayparam = par("delay");
    if (delayparam<0)
        throw cRuntimeError(this, "negative delay %s", SIMTIME_STR(delayparam));
    setFlag(FL_ISDISABLED, par("disabled"));
    setFlag(FL_DELAY_NONZERO, delayparam!=0);
}

void cDelayChannel::handleParameterChange(const char *)
{
    rereadPars();
}

void cDelayChannel::setDelay(double d)
{
    par("delay").setDoubleValue(d);
}

void cDelayChannel::setDisabled(bool d)
{
    par("disabled").setBoolValue(d);
}

bool cDelayChannel::deliver(cMessage *msg, simtime_t t)
{
    // if channel is disabled, signal that message should be deleted
    if (flags & FL_ISDISABLED)
        return false;

    // propagation delay modeling
    if (flags & FL_DELAY_NONZERO)
        t += delayparam;

    // FIXME: this is not reusable this way in custom channels, put it into a base class function with the next line (levy)
    // i.e use template method...?
    EVCB.messageSendHop(msg, getSourceGate(), delayparam, SIMTIME_ZERO);

    // hand over msg to next gate
    cGate *nextgate = getSourceGate()->getNextGate();
    return nextgate->deliver(msg, t);
}

