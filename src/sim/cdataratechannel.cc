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

    rereadPars();

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
    return channelFinishTime;
}

bool cDatarateChannel::isBusy() const
{
    if (mode == UNCHECKED)
        throw cRuntimeError(this, "isBusy() is unsupported in mode=UNCHECKED");
    return channelFinishTime > getSimulation()->getSimTime();
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
    Tx *tx = nullptr;

    // channel must be idle, or in the case of tx update, referenced tx must still be in progress
    if (mode == SINGLE) {
        tx = &singleTx;
        if (!isUpdate) {
            if (t < tx->finishTime)
                throw cRuntimeError("Cannot send packet (%s)%s: Channel is currently busy with an ongoing transmission -- "
                        "please rewrite the sender module to only send when the previous transmission has already finished, "
                        "using cGate::getTransmissionFinishTime(), scheduleAt(), and possibly a cPacketQueue for storing packets "
                        "waiting to be transmitted", pkt->getClassName(), pkt->getFullName());
        }
        else {
            ASSERT(pkt->getTransmissionId() != -1); // assured in send()
            if (pkt->getTransmissionId() != tx->transmissionId)
                throw cRuntimeError("Cannot send transmission update packet (%s)%s: transmissionId=%" PRId64 " does not match that of the last transmission on the channel", pkt->getClassName(), pkt->getFullName(), pkt->getTransmissionId());
            if (t > tx->finishTime) // note: if they are equal and it's a problem, we'll catch that in cSimpleModule::deleteObsoletedTransmissionFromFES()
                throw cRuntimeError("Cannot send transmission update packet (%s)%s: It has missed the end of the transmission to be updated", pkt->getClassName(), pkt->getFullName());
        }
    }
    else if (mode == MULTI) {
        if (!isUpdate) {
            txList.push_back(Tx());
            tx = &txList.back();
            tx->transmissionId = pkt->getTransmissionId();
        }
        else {
            // find updated transmission, purge expired ones
            ASSERT(pkt->getTransmissionId() != -1); // assured in send()
            int txIndex = -1;
            simtime_t now = getSimulation()->getSimTime();
            for (int i = 0; i < txList.size(); i++) {
                if (txList[i].finishTime < now) {
                    txList[i] = txList.back(); // no-op if txList[i] is the last item (i.e. txList.back())
                    txList.pop_back();
                    i--;
                }
                else if (pkt->getTransmissionId() == txList[i].transmissionId)
                    txIndex = i;
            }
            if (txIndex == -1)
                throw cRuntimeError("Cannot send transmission update packet (%s)%s: The transmission transmissionId=%" PRId64 " references was not found (either nonexistent or already finished)", pkt->getClassName(), pkt->getFullName(), pkt->getTransmissionId());
            tx = &txList[txIndex];
        }
    }

    // if channel is disabled, signal that message should be deleted; however, tx updates must
    // be allowed to go through to make it possible for the transmitter module to abort the
    // ongoing packet transmission.
    if (flags & FL_ISDISABLED && !isUpdate) {
        result.discard = true;
        return;
    }

    // indicate that packet has traversed a transmission channel
    pkt->setTxChannelEncountered();

    // datarate modeling

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
        if (mode != UNCHECKED) {
            tx->transmissionId = pkt->getTransmissionId();
            tx->startTime = t;
            tx->finishTime = t + duration;
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
        else {
            simtime_t elapsedTxTime = t - tx->startTime;

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

            tx->finishTime = t + remainingDuration;
        }
    }

    pkt->setDuration(duration);
    pkt->setRemainingDuration(remainingDuration);

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

    // update channelFinishTime
    simtime_t oldFinishTime = channelFinishTime;
    if (mode == SINGLE)
        channelFinishTime = tx->finishTime;
    else if (mode == MULTI) {
        simtime_t tmax = SIMTIME_ZERO;
        for (const Tx& tx: txList)
            if (tx.finishTime > tmax)
                tmax = tx.finishTime;
        channelFinishTime = tmax;
    }

    // emit busySignal
    if (mode != UNCHECKED && mayHaveListeners(channelBusySignal)) {
        if (oldFinishTime < t) {
            cTimestampedValue tmp(oldFinishTime, (intval_t)0);
            emit(channelBusySignal, &tmp);
        }
        if (channelFinishTime > t) {
            cTimestampedValue tmp(t, (intval_t)1);
            emit(channelBusySignal, &tmp);
        }
    }
}

void cDatarateChannel::finish()
{
    if (mode != UNCHECKED && mayHaveListeners(channelBusySignal)) {
        cTimestampedValue tmp(std::min(channelFinishTime, simTime()), (intval_t)0);
        emit(channelBusySignal, &tmp);
    }
}

void cDatarateChannel::forceTransmissionFinishTime(simtime_t t)
{
    if (mode != SINGLE)
        throw cRuntimeError(this, "forceTransmissionFinishTime() may only be used in mode=SINGLE");
    if (singleTx.finishTime > t)
        singleTx.finishTime = t;
    channelFinishTime = t;
}

}  // namespace omnetpp

