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
    txfinishtime = 0;
    delayparam = 0;
    errorparam = 0;
    datarateparam = 0;
}

cBasicChannel::~cBasicChannel()
{
}

std::string cBasicChannel::info() const
{
    return cChannel::info();
}

void cBasicChannel::finalizeParameters()
{
    cChannel::finalizeParameters();
    rereadPars();
}

void cBasicChannel::rereadPars()
{
    delayparam = par("delay");
    errorparam = par("error");
    datarateparam = par("datarate");

    if (delayparam<0)
        throw cRuntimeError(this, "negative delay %s", SIMTIME_STR(delayparam));
    if (errorparam<0 || errorparam>1)
        throw cRuntimeError(this,"wrong bit error rate %g", errorparam);
    if (datarateparam<0)
        throw cRuntimeError(this, "negative datarate %g", datarateparam);

    setFlag(FL_ISDISABLED, par("disabled"));
    setFlag(FL_DELAY_NONZERO, delayparam!=0);
    setFlag(FL_ERROR_NONZERO, errorparam!=0);
    setFlag(FL_DATARATE_NONZERO, datarateparam!=0);
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
    return simulation.simTime() < txfinishtime;
}

bool cBasicChannel::deliver(cMessage *msg, simtime_t t)
{
    if (!areParamsFinalized())
        throw cRuntimeError("Error sending message (%s)%s on gate %s: channel parameters not yet set up",
                            msg->className(), msg->fullName(), fromGate()->fullPath().c_str());

    // if channel is disabled, signal that message should be deleted
    if (flags & FL_ISDISABLED)
        return false;

    // must wait until previous transmissions end
    if (txfinishtime > t)
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
        transmissiondelay = msg->length() / datarateparam;
        t += transmissiondelay;
        txfinishtime = t;
    }

    // propagation delay modeling
    if (flags & FL_DELAY_NONZERO)
    {
        t += delayparam;
    }

    // bit error rate modeling
    if (flags & FL_ERROR_NONZERO)
    {
        if (dblrand() < 1.0 - pow(1.0-errorparam, (double)msg->length()))
            msg->setBitError(true);
    }

    // FIXME: XXX: this is not reusable this way in custom channels, put it into a base class function with the next line (levy)
    EVCB.messageSendHop(msg, fromGate(), delayparam, transmissiondelay);

    // hand over msg to next gate
    return fromGate()->toGate()->deliver(msg, t);
}

