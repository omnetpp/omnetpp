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

void cDatarateChannel::setMode(Mode mode)
{
    this->mode = mode;
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

simtime_t cDatarateChannel::getTransmissionFinishTime() const
{
    if (mode == UNCHECKED)
        throw cRuntimeError(this, "getTransmissionFinishTime() is unsupported in mode=UNCHECKED");
    return txFinishTime;
}

bool cDatarateChannel::isBusy() const
{
    if (mode == UNCHECKED)
        throw cRuntimeError(this, "isBusy() is unsupported in mode=UNCHECKED");
    return simTime() < txFinishTime;
}

cChannel::Result cDatarateChannel::processMessage(cMessage *msg, const SendOptions& options, simtime_t t)
{
    Result result;

    if (msg->isPacket()) {
        // disabled channel, transmission duration, and error modeling
        processPacket(static_cast<cPacket *>(msg), options, t, result);
    }
    else { // non-packet messages
        if (flags & FL_ISDISABLED)
            result.discard = true;
    }

    // propagation delay modeling
    result.delay = delay;

    if (!result.discard) {
        if (mayHaveListeners(messageSentSignal)) {
            MessageSentSignalValue tmp(t, msg, &result);
            emit(messageSentSignal, &tmp);
        }
    }
    else {
        if (mayHaveListeners(messageDiscardedSignal)) {
            cTimestampedValue tmp(t, msg);
            emit(messageDiscardedSignal, &tmp);
        }
    }

    return result;
}

void cDatarateChannel::processPacket(cPacket *pkt, const SendOptions& options, simtime_t t, Result& result)
{
    bool isUpdate = pkt->isUpdate();

    // channel must be idle, or in the case of tx update, referenced tx must still be in progress
    if (mode == SINGLE) {
        if (!isUpdate) {
            if (t < txFinishTime)
                throw cRuntimeError("Cannot send packet (%s)%s: Channel is currently busy with an ongoing transmission -- "
                        "please rewrite the sender module to only send when the previous transmission has already finished, "
                        "using cGate::getTransmissionFinishTime(), scheduleAt(), and possibly a cPacketQueue for storing packets "
                        "waiting to be transmitted", pkt->getClassName(), pkt->getFullName());
        }
        else {
            if (pkt->getOrigPacketId() != lastOrigPacketId)
                throw cRuntimeError("Cannot send transmission update packet (%s)%s: origPacketId=%ld does not match that of the last transmission on the channel", pkt->getClassName(), pkt->getFullName(), pkt->getOrigPacketId());
            if (t > txFinishTime) // note: if they are equal and it's a problem, we'll catch that in cSimpleModule::deleteObsoletedTransmissionFromFES()
                throw cRuntimeError("Cannot send transmission update packet (%s)%s: It has missed the end of the transmission to be updated", pkt->getClassName(), pkt->getFullName());
        }
    }

    // if channel is disabled, signal that message should be deleted; however, tx updates must
    // be allowed to go through to make it possible for the transmitter module to abort the
    // ongoing packet transmission.
    if (flags & FL_ISDISABLED && !isUpdate) {
        result.discard = true;
        return;
    }

    // datarate modeling

    // signal end of previous transmission (unless this is a tx update)
    if (mode == SINGLE) {
        if (!isUpdate && txFinishTime != -1 && mayHaveListeners(channelBusySignal)) {
            cTimestampedValue tmp(txFinishTime, (intval_t)0);
            emit(channelBusySignal, &tmp);
        }
    }

    // indicate that packet has traversed a transmission channel
    pkt->setTxChannelEncountered();

    // compute duration and remainingDuration
    simtime_t duration;
    simtime_t remainingDuration;
    bool durationSpecified  = options.duration_ != SendOptions::DURATION_UNSPEC;
    if (!isUpdate) {
        if (durationSpecified) {
            duration = options.duration_;
            if (duration < SIMTIME_ZERO)
                throw cRuntimeError(this, "Cannot send packet (%s)%s: Negative duration (%s) specified", pkt->getClassName(), pkt->getName(), duration.ustr().c_str());
        }
        else if (flags & FL_DATARATE_PRESENT)
            duration = pkt->getBitLength() / datarate;
        else
            throw cRuntimeError(this, "Cannot send packet (%s)%s: Unknown duration: No channel datarate, and no explicit duration supplied in the send call", pkt->getClassName(), pkt->getName());
        if (mode == SINGLE) {
            lastOrigPacketId = pkt->getId();
            txStartTime = t;
        }
        remainingDuration = duration;
    }
    else { // update
        bool remainingDurationSpecified  = options.remainingDuration != SendOptions::DURATION_UNSPEC;
        if (durationSpecified) {
            duration = options.duration_;
            if (duration < SIMTIME_ZERO)
                throw cRuntimeError(this, "Cannot send transmission update packet (%s)%s: Negative duration (%s) specified", pkt->getClassName(), pkt->getName(), duration.ustr().c_str());
        }

        if (remainingDurationSpecified) {
            remainingDuration = options.remainingDuration;
            if (remainingDuration < SIMTIME_ZERO)
                throw cRuntimeError(this, "Cannot send transmission update packet (%s)%s: Negative remainingDuration (%s) specified", pkt->getClassName(), pkt->getName(), remainingDuration.ustr().c_str());
        }

        if (mode == UNCHECKED) {
            if (!durationSpecified || !remainingDurationSpecified)
                throw cRuntimeError(this, "Cannot send transmission update packet (%s)%s: Either duration or remainingDuration is unspecified", pkt->getClassName(), pkt->getName());
        }
        else if (mode == SINGLE) {
            simtime_t elapsedTxTime = t - txStartTime;

            if (durationSpecified && remainingDurationSpecified) {
                if (duration != elapsedTxTime + remainingDuration)
                    throw cRuntimeError(this, "Cannot send transmission update packet (%s)%s: Both duration and remainingDuration are specified, and duration (%s) != elapsedTxTime (%s) + remainingDuration (%s)",
                            pkt->getClassName(), pkt->getName(), duration.ustr().c_str(), elapsedTxTime.ustr().c_str(), remainingDuration.ustr().c_str());
            }
            else if (durationSpecified) { // but not remainingDuration
                remainingDuration = duration - elapsedTxTime;
                if (remainingDuration < SIMTIME_ZERO)
                    throw cRuntimeError(this, "Cannot send transmission update packet (%s)%s: Duration (%s) is smaller than already elapsed transmission time (%s)", pkt->getClassName(), pkt->getName(), duration.ustr().c_str(), elapsedTxTime.ustr().c_str());
            }
            else if (remainingDurationSpecified) { // but not duration
                duration = elapsedTxTime + remainingDuration;
            }
            else { // neither
                if (!(flags & FL_DATARATE_PRESENT))
                    throw cRuntimeError(this, "Cannot send transmission update packet (%s)%s: Unknown duration: No channel datarate and no explicit duration or remainingDuration supplied in the send call", pkt->getClassName(), pkt->getName());
                duration = pkt->getBitLength() / datarate;
                remainingDuration = duration - elapsedTxTime;
            }
        }
    }

    pkt->setDuration(duration);
    pkt->setRemainingDuration(remainingDuration);
    if (mode == SINGLE)
        txFinishTime = t + remainingDuration;

    result.duration = duration;
    result.remainingDuration = remainingDuration;

    // bit error modeling
    if (flags & (FL_BER_NONZERO | FL_PER_NONZERO)) {
        if (flags & FL_BER_NONZERO)
            if (dblrand() < 1.0 - pow(1.0 - ber, (double)pkt->getBitLength()))
                pkt->setBitError(true);

        if (flags & FL_PER_NONZERO)
            if (dblrand() < per)
                pkt->setBitError(true);
    }

    if (mode == SINGLE && mayHaveListeners(channelBusySignal)) {
        cTimestampedValue tmp(t, (intval_t)1);
        emit(channelBusySignal, &tmp);
    }
}

void cDatarateChannel::finish()
{
    if (mode == SINGLE && txFinishTime != -1 && mayHaveListeners(channelBusySignal)) {
        cTimestampedValue tmp(std::min(txFinishTime, simTime()), (intval_t)0);
        emit(channelBusySignal, &tmp);
    }
}

}  // namespace omnetpp

