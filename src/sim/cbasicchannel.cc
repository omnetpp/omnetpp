//========================================================================
//  CBASICCHANNEL.CC - part of
//
//                 OMNeT++/OMNEST
//              Discrete System Simulation in C++
//
//========================================================================

/*--------------------------------------------------------------*
  Copyright (C) 1992-2005 Andras Varga

  This file is distributed WITHOUT ANY WARRANTY. See the file
  `license' for details on this and other legal matters.
*--------------------------------------------------------------*/

#include "cbasicchannel.h"
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

Register_Class(cBasicChannel);


cBasicChannel::cBasicChannel(const char *name) : cChannel(name)
{
    transm_finishes = 0;
    delay_ = 0;
    error_ = 0;
    datarate_ = 0;
}

cBasicChannel::~cBasicChannel()
{
}

std::string cBasicChannel::info() const
{
    return cChannel::info();
}

void cBasicChannel::initialize()
{
    rereadPars();
    flags |= FL_INITIALIZED;
}

void cBasicChannel::rereadPars()
{
    delay_ = par("delay");
    error_ = par("error");
    datarate_ = par("datarate");

    if (delay_<0)
        throw cRuntimeError(this, "negative delay %s", SIMTIME_STR(delay_));
    if (error_<0 || error_>1)
        throw cRuntimeError(this,"wrong bit error rate %g", error_);
    if (datarate_<0)
        throw cRuntimeError(this, "negative datarate %g", datarate_);

    flags &= ~FL_BASICCHANNELFLAGS;
    if (par("disabled")) flags |= FL_ISDISABLED;
    if (delay_!=0) flags |= FL_DELAY_NONZERO;
    if (error_!=0) flags |= FL_ERROR_NONZERO;
    if (datarate_!=0) flags |= FL_DATARATE_NONZERO;
}

void cBasicChannel::handleParameterChange(const char *)
{
    rereadPars();
}

void cBasicChannel::setDelay(double d)
{
    par("delay").setDoubleValue(d);
}

void cBasicChannel::setError(double d)
{
    par("error").setDoubleValue(d);
}

void cBasicChannel::setDatarate(double d)
{
    par("datarate").setDoubleValue(d);
}

void cBasicChannel::setDisabled(bool d)
{
    par("disabled").setBoolValue(d);
}

bool cBasicChannel::isBusy() const
{
    return simulation.simTime() < transm_finishes;
}

bool cBasicChannel::deliver(cMessage *msg, simtime_t t)
{
    checkInitialized();

    // if channel is disabled, signal that message should be deleted
    if (flags & FL_ISDISABLED)
        return false;

    // must wait until previous transmissions end
    if (transm_finishes > t)
        throw cRuntimeError("Error sending message (%s)%s on gate %s: gate is currently "
                            "busy with an ongoing transmission -- please rewrite the sender "
                            "simple module to only send when the previous transmission has "
                            "already finished, using cGate::transmissionFinishes(), scheduleAt(), "
                            "and possibly a cQueue for storing messages waiting to be transmitted",
                            msg->className(), msg->fullName(), fromGate()->fullPath().c_str());

    simtime_t transmissiondelay = 0;

    // datarate modeling
    if (flags & FL_DATARATE_NONZERO)
    {
        transmissiondelay = msg->length() / datarate_;
        t += transmissiondelay;
        transm_finishes = t;
    }

    // propagation delay modeling
    if (flags & FL_DELAY_NONZERO)
    {
        t += delay_;
    }

    // bit error rate modeling
    if (flags & FL_ERROR_NONZERO)
    {
        if (dblrand() < 1.0 - pow(1.0-error_, msg->length()))
            msg->setBitError(true);
    }

    // FIXME: XXX: this is not reusable this way in custom channels, put it into a base class function with the next line (levy)
    EVCB.messageSendHop(msg, fromGate(), delay_, transmissiondelay);

    // hand over msg to next gate
    return fromGate()->toGate()->deliver(msg, t);
}

