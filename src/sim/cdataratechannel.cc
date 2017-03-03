//========================================================================
//  CDATARATECHANNEL.CC - part of
//
//                 OMNeT++/OMNEST
//              Discrete System Simulation in C++
//
//========================================================================

/*--------------------------------------------------------------*
  Copyright (C) 1992-2017 Andras Varga
  Copyright (C) 2006-2017 OpenSim Ltd.

  This file is distributed WITHOUT ANY WARRANTY. See the file
  `license' for details on this and other legal matters.
*--------------------------------------------------------------*/

#include "omnetpp/cdataratechannel.h"
#include "omnetpp/cpacket.h"
#include "omnetpp/cmodule.h"
#include "omnetpp/cenvir.h"
#include "omnetpp/distrib.h"
#include "omnetpp/csimulation.h"
#include "omnetpp/globals.h"
#include "omnetpp/cgate.h"
#include "omnetpp/cexception.h"
#include "omnetpp/ctimestampedvalue.h"

#ifdef WITH_PARSIM
#include "omnetpp/ccommbuffer.h"
#endif

namespace omnetpp {

using std::ostream;

Register_Class(cDatarateChannel);

simsignal_t cDatarateChannel::channelBusySignal;
simsignal_t cDatarateChannel::messageSentSignal;
simsignal_t cDatarateChannel::messageDiscardedSignal;

cDatarateChannel::cDatarateChannel(const char *name) : cChannel(name)
{
    txFinishTime = -1;
    delay = 0;
    datarate = 0;
    ber = 0;
    per = 0;
}

cDatarateChannel::~cDatarateChannel()
{
}

void cDatarateChannel::finish()
{
    if (txFinishTime != -1 && mayHaveListeners(channelBusySignal)) {
        cTimestampedValue tmp(txFinishTime, 0L);
        emit(channelBusySignal, &tmp);
    }
}

cDatarateChannel *cDatarateChannel::create(const char *name)
{
    return dynamic_cast<cDatarateChannel *>(cChannelType::getDatarateChannelType()->create(name));
}

std::string cDatarateChannel::str() const
{
    return cChannel::str();
}

void cDatarateChannel::initialize()
{
    channelBusySignal = registerSignal("channelBusy");
    messageSentSignal = registerSignal("messageSent");
    messageDiscardedSignal = registerSignal("messageDiscarded");

    emit(channelBusySignal, 0);
}

void cDatarateChannel::rereadPars()
{
    delay = par("delay");
    datarate = par("datarate");
    ber = par("ber");
    per = par("per");

    if (delay < SIMTIME_ZERO)
        throw cRuntimeError(this, "Negative delay %s", SIMTIME_STR(delay));
    if (datarate < 0)
        throw cRuntimeError(this, "Negative datarate %g", datarate);
    if (ber < 0 || ber > 1)
        throw cRuntimeError(this, "Wrong bit error rate %g", ber);
    if (per < 0 || per > 1)
        throw cRuntimeError(this, "Wrong packet error rate %g", per);

    setFlag(FL_ISDISABLED, par("disabled"));
    setFlag(FL_DELAY_NONZERO, delay != SIMTIME_ZERO);
    setFlag(FL_DATARATE_NONZERO, datarate != 0);
    setFlag(FL_BER_NONZERO, ber != 0);
    setFlag(FL_PER_NONZERO, per != 0);
}

void cDatarateChannel::handleParameterChange(const char *)
{
    rereadPars();
}

void cDatarateChannel::setDelay(double d)
{
    par("delay").setDoubleValue(d);
}

void cDatarateChannel::setDatarate(double d)
{
    par("datarate").setDoubleValue(d);
}

void cDatarateChannel::setBitErrorRate(double d)
{
    par("ber").setDoubleValue(d);
}

void cDatarateChannel::setPacketErrorRate(double d)
{
    par("per").setDoubleValue(d);
}

void cDatarateChannel::setDisabled(bool d)
{
    par("disabled").setBoolValue(d);
}

simtime_t cDatarateChannel::calculateDuration(cMessage *msg) const
{
    if ((flags & FL_DATARATE_NONZERO) && msg->isPacket())
        return ((cPacket *)msg)->getBitLength() / datarate;
    else
        return SIMTIME_ZERO;
}

void cDatarateChannel::processMessage(cMessage *msg, simtime_t t, result_t& result)
{
    // if channel is disabled, signal that message should be deleted
    if (flags & FL_ISDISABLED) {
        result.discard = true;
        cTimestampedValue tmp(t, msg);
        emit(messageDiscardedSignal, &tmp);
        return;
    }

    if (txFinishTime != -1 && mayHaveListeners(channelBusySignal)) {
        cTimestampedValue tmp(txFinishTime, 0L);
        emit(channelBusySignal, &tmp);
    }

    // datarate modeling
    if ((flags & FL_DATARATE_NONZERO) && msg->isPacket()) {
        cPacket *pkt = (cPacket *)msg;
        simtime_t duration = pkt->getBitLength() / datarate;
        result.duration = duration;
        txFinishTime = t + duration;
    }
    else {
        txFinishTime = t;
    }

    // propagation delay modeling
    result.delay = delay;

    // bit error modeling
    if ((flags & (FL_BER_NONZERO | FL_PER_NONZERO)) && msg->isPacket()) {
        cPacket *pkt = (cPacket *)msg;
        if (flags & FL_BER_NONZERO)
            if (dblrand() < 1.0 - pow(1.0 - ber, (double)pkt->getBitLength()))
                pkt->setBitError(true);

        if (flags & FL_PER_NONZERO)
            if (dblrand() < per)
                pkt->setBitError(true);
    }

    // emit signals
    if (mayHaveListeners(messageSentSignal)) {
        MessageSentSignalValue tmp(t, msg, &result);
        emit(messageSentSignal, &tmp);
    }
    if (mayHaveListeners(channelBusySignal)) {
        cTimestampedValue tmp(t, 1L);
        emit(channelBusySignal, &tmp);
    }
}

void cDatarateChannel::forceTransmissionFinishTime(simtime_t t)
{
    //TODO record this into the eventlog so that it can be visualized in the sequence chart
    txFinishTime = t;
}

}  // namespace omnetpp

