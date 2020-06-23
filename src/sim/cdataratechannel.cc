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
#include "omnetpp/csimplemodule.h" // SendOptions

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
        cTimestampedValue tmp(std::min(txFinishTime, simTime()), (intval_t)0);
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

    bool hasDatarate = !std::isnan(datarate) && datarate != 0;
    if (delay < SIMTIME_ZERO)
        throw cRuntimeError(this, "Invalid delay %s", SIMTIME_STR(delay));
    if (hasDatarate && (!std::isfinite(datarate) || datarate < 0))
        throw cRuntimeError(this, "Invalid datarate %g", datarate);
    if (!std::isfinite(ber) || ber < 0 || ber > 1)
        throw cRuntimeError(this, "Invalid bit error rate %g", ber);
    if (!std::isfinite(per) || per < 0 || per > 1)
        throw cRuntimeError(this, "Invalid packet error rate %g", per);

    setFlag(FL_ISDISABLED, par("disabled"));
    setFlag(FL_DELAY_NONZERO, delay != SIMTIME_ZERO);
    setFlag(FL_DATARATE_PRESENT, hasDatarate);
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
    if ((flags & FL_DATARATE_PRESENT) && msg->isPacket())
        return ((cPacket *)msg)->getBitLength() / datarate;
    else
        return SIMTIME_ZERO;
}

cChannel::Result cDatarateChannel::processMessage(cMessage *msg, const SendOptions& options, simtime_t t)
{
    Result result;

    bool isPacket = msg->isPacket();
    cPacket *pkt = isPacket ? static_cast<cPacket *>(msg) : nullptr;
    bool isUpdate = isPacket ? pkt->isUpdate() : false;

    // channel must be idle, or in the case of tx update, referenced tx must still be in progress
    if (isPacket) {
        if (isUpdate) {
            if (pkt->getOrigPacketId() != lastOrigPacketId)
                throw cRuntimeError("Cannot send transmission update packet (%s)%s: origPacketId=%ld does not match that of the last transmission on the channel", msg->getClassName(), msg->getFullName(), pkt->getOrigPacketId());
            if (t > txFinishTime) // note: if they are equal and it's a problem, we'll catch that in cSimpleModule::deleteObsoletedTransmissionFromFES()
                throw cRuntimeError("Cannot send transmission update packet (%s)%s: It has missed the end of the transmission to be updated", msg->getClassName(), msg->getFullName());
        }
        else {
            if (t < txFinishTime)
                throw cRuntimeError("Cannot send packet (%s)%s: Channel is currently busy with an ongoing transmission -- "
                        "please rewrite the sender module to only send when the previous transmission has already finished, "
                        "using cGate::getTransmissionFinishTime(), scheduleAt(), and possibly a cPacketQueue for storing packets "
                        "waiting to be transmitted", msg->getClassName(), msg->getFullName());
        }
    }

    // if channel is disabled, signal that message should be deleted; however, tx updates must
    // be allowed to go through to make it possible for the transmitter module to abort the
    // ongoing packet transmission.
    if (flags & FL_ISDISABLED && !isUpdate) {
        result.discard = true;
        cTimestampedValue tmp(t, msg);
        emit(messageDiscardedSignal, &tmp);
        return result;
    }

    // datarate modeling
    if (isPacket) {
        // signal end of previous transmission (unless this is a tx update)
        if (!isUpdate && txFinishTime != -1 && mayHaveListeners(channelBusySignal)) {
            cTimestampedValue tmp(txFinishTime, (intval_t)0);
            emit(channelBusySignal, &tmp);
        }

        // indicate that packet has traversed a transmission channel
        pkt->setTxChannelEncountered();

        // compute duration
        simtime_t duration;
        if (options.duration_ != SendOptions::DURATION_UNSPEC) {
            duration = options.duration_;
            if (duration < SIMTIME_ZERO)
                throw cRuntimeError(this, "Cannot send packet (%s)%s: Negative duration (%s) specified", msg->getClassName(), msg->getName(), duration.ustr().c_str());
        }
        else if (flags & FL_DATARATE_PRESENT)
            duration = pkt->getBitLength() / datarate;
        else if (isUpdate)
            duration = SendOptions::DURATION_UNSPEC; // we might be able to compute it as elapsed+remaining transmission time
        else
            throw cRuntimeError(this, "Cannot send packet (%s)%s: Unknown duration: No channel datarate, and no explicit duration supplied in the send call", msg->getClassName(), msg->getName());

        // compute remainingDuration
        simtime_t remainingDuration;
        if (!isUpdate) {
            lastOrigPacketId = pkt->getId();
            txStartTime = t;
            remainingDuration = duration;
        }
        else {
            simtime_t elapsedTxTime = t - txStartTime;
            if (options.remainingDuration == SendOptions::DURATION_UNSPEC) {
                if (duration == SendOptions::DURATION_UNSPEC)
                    throw cRuntimeError(this, "Cannot send transmission update packet (%s)%s: Unknown duration: No channel datarate and no explicit duration or remainingDuration supplied in the send call", msg->getClassName(), msg->getName());
                remainingDuration = duration - elapsedTxTime;
                if (remainingDuration < SIMTIME_ZERO)
                    throw cRuntimeError(this, "Cannot send transmission update packet (%s)%s: Duration (%s) is smaller than already elapsed transmission time (%s)", msg->getClassName(), msg->getName(), duration.ustr().c_str(), elapsedTxTime.ustr().c_str());
            }
            else {
                remainingDuration = options.remainingDuration;
                if (remainingDuration < SIMTIME_ZERO)
                    throw cRuntimeError(this, "Cannot send transmission update packet (%s)%s: Negative remainingDuration (%s) specified", msg->getClassName(), msg->getName(), remainingDuration.ustr().c_str());
                if (duration == SendOptions::DURATION_UNSPEC)
                    duration = elapsedTxTime + remainingDuration;
                else if (duration != elapsedTxTime + remainingDuration)
                    throw cRuntimeError(this, "Cannot send transmission update packet (%s)%s: Both duration and remainingDuration are specified, and duration (%s) != elapsedTxTime (%s) + remainingDuration (%s)",
                            msg->getClassName(), msg->getName(), duration.ustr().c_str(), elapsedTxTime.ustr().c_str(), remainingDuration.ustr().c_str());
            }
        }

        pkt->setDuration(duration);
        pkt->setRemainingDuration(remainingDuration);
        txFinishTime = t + remainingDuration;

        result.duration = duration;
        result.remainingDuration = remainingDuration;
    }

    // propagation delay modeling
    result.delay = delay;

    // bit error modeling
    if (isPacket && (flags & (FL_BER_NONZERO | FL_PER_NONZERO))) {
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
        cTimestampedValue tmp(t, (intval_t)1);
        emit(channelBusySignal, &tmp);
    }

    return result;
}

}  // namespace omnetpp

