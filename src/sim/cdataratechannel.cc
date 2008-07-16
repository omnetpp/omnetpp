//========================================================================
//  CDATARATECHANNEL.CC - part of
//
//                 OMNeT++/OMNEST
//              Discrete System Simulation in C++
//
//========================================================================

/*--------------------------------------------------------------*
  Copyright (C) 1992-2008 Andras Varga
  Copyright (C) 2006-2008 OpenSim Ltd.

  This file is distributed WITHOUT ANY WARRANTY. See the file
  `license' for details on this and other legal matters.
*--------------------------------------------------------------*/

#include "cdataratechannel.h"
#include "cmessage.h"
#include "cmodule.h"
#include "cenvir.h"
#include "random.h"
#include "distrib.h"
#include "csimulation.h"
#include "globals.h"
#include "cgate.h"
#include "cexception.h"

#ifdef WITH_PARSIM
#include "ccommbuffer.h"
#endif

USING_NAMESPACE

using std::ostream;

Register_Class(cDatarateChannel);


cDatarateChannel::cDatarateChannel(const char *name) : cChannel(name)
{
    txfinishtime = 0;
    delayparam = 0;
    datarateparam = 0;
    berparam = 0;
    perparam = 0;
}

cDatarateChannel::~cDatarateChannel()
{
}

std::string cDatarateChannel::info() const
{
    return cChannel::info();
}

void cDatarateChannel::finalizeParameters()
{
    cChannel::finalizeParameters();
    rereadPars();
}

void cDatarateChannel::rereadPars()
{
    delayparam = par("delay");
    datarateparam = par("datarate");
    berparam = par("ber");
    perparam = par("per");

    if (delayparam<0)
        throw cRuntimeError(this, "negative delay %s", SIMTIME_STR(delayparam));
    if (datarateparam<0)
        throw cRuntimeError(this, "negative datarate %g", datarateparam);
    if (berparam<0 || berparam>1)
        throw cRuntimeError(this, "wrong bit error rate %g", berparam);
    if (perparam<0 || perparam>1)
        throw cRuntimeError(this, "wrong packet error rate %g", perparam);

    setFlag(FL_ISDISABLED, par("disabled"));
    setFlag(FL_DELAY_NONZERO, delayparam!=0);
    setFlag(FL_DATARATE_NONZERO, datarateparam!=0);
    setFlag(FL_BER_NONZERO, berparam!=0);
    setFlag(FL_PER_NONZERO, perparam!=0);
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

bool cDatarateChannel::isBusy() const
{
    return simulation.getSimTime() < txfinishtime;
}

bool cDatarateChannel::deliver(cMessage *msg, simtime_t t)
{
    // if channel is disabled, signal that message should be deleted
    if (flags & FL_ISDISABLED)
        return false;

    // must wait until previous transmissions end
    if (txfinishtime > t)
        throw cRuntimeError("Error sending message (%s)%s on gate %s: gate is currently "
                            "busy with an ongoing transmission -- please rewrite the sender "
                            "simple module to only send when the previous transmission has "
                            "already finished, using cGate::getTransmissionFinishTime(), scheduleAt(), "
                            "and possibly a cQueue for storing messages waiting to be transmitted",
                            msg->getClassName(), msg->getFullName(), getFromGate()->getFullPath().c_str());

    cGate *nextgate = getFromGate()->getToGate();

    simtime_t duration = 0;
    bool isstart = false;

    // datarate modeling
    if (flags & FL_DATARATE_NONZERO)
    {
        duration = msg->getBitLength() / datarateparam;
        isstart = nextgate->getDeliverOnReceptionStart();
        msg->setDuration(duration);
        msg->setReceptionStart(isstart);
        if (!isstart)
            t += duration;
        txfinishtime = t;
    }

    // propagation delay modeling
    if (flags & FL_DELAY_NONZERO)
    {
        t += delayparam;
    }

    // bit error modeling
    if (flags & (FL_BER_NONZERO | FL_PER_NONZERO))
    {
        if (flags & FL_BER_NONZERO)
            if (dblrand() < 1.0 - pow(1.0-berparam, (double)msg->getBitLength()))
                msg->setBitError(true);
        if (flags & FL_PER_NONZERO)
            if (dblrand() < perparam)
                msg->setBitError(true);
    }

    // FIXME: this is not reusable this way in custom channels, put it into a base class function with the next line (levy)
    // i.e use template method...?
    EVCB.messageSendHop(msg, getFromGate(), delayparam, duration, isstart);

    // hand over msg to next gate
    return nextgate->deliver(msg, t);
}

