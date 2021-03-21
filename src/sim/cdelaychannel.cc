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
  Copyright (C) 1992-2017 Andras Varga
  Copyright (C) 2006-2017 OpenSim Ltd.

  This file is distributed WITHOUT ANY WARRANTY. See the file
  `license' for details on this and other legal matters.
*--------------------------------------------------------------*/

#include "omnetpp/cdelaychannel.h"
#include "omnetpp/cmessage.h"
#include "omnetpp/cenvir.h"
#include "omnetpp/globals.h"
#include "omnetpp/cgate.h"
#include "omnetpp/cexception.h"
#include "omnetpp/ctimestampedvalue.h"

#ifdef WITH_PARSIM
#include "omnetpp/ccommbuffer.h"
#endif

namespace omnetpp {

using std::ostream;

Register_Class(cDelayChannel);

simsignal_t cDelayChannel::messageSentSignal;
simsignal_t cDelayChannel::messageDiscardedSignal;

cDelayChannel *cDelayChannel::create(const char *name)
{
    return dynamic_cast<cDelayChannel *>(cChannelType::getDelayChannelType()->create(name));
}

void cDelayChannel::initialize()
{
    messageSentSignal = registerSignal("messageSent");
    messageDiscardedSignal = registerSignal("messageDiscarded");

    rereadPars();
}

void cDelayChannel::rereadPars()
{
    delay = par("delay");
    if (delay < 0)
        throw cRuntimeError(this, "Negative delay %s", SIMTIME_STR(delay));
    setFlag(FL_ISDISABLED, par("disabled"));
    setFlag(FL_DELAY_NONZERO, delay != 0);
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

cChannel::Result cDelayChannel::processMessage(cMessage *msg, const SendOptions& options, simtime_t t)
{
    Result result;

    // if channel is disabled, signal that message should be deleted
    if (flags & FL_ISDISABLED) {
        result.discard = true;
        cTimestampedValue tmp(t, msg);
        emit(messageDiscardedSignal, &tmp);
        return result;
    }

    // propagation delay modeling
    result.delay = delay;

    // emit signals
    if (mayHaveListeners(messageSentSignal)) {
        MessageSentSignalValue tmp(t, msg, &result);
        emit(messageSentSignal, &tmp);
    }

    return result;
}

}  // namespace omnetpp

