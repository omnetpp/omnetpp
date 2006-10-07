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

using std::ostream;

Register_Class(cBasicChannel);


cBasicChannel::cBasicChannel(const char *name) : cChannel(name)
{
    transm_finishes = 0.0;
}

cBasicChannel::~cBasicChannel()
{
}

std::string cBasicChannel::info() const
{
    return cChannel::info();
}

void cBasicChannel::netPack(cCommBuffer *buffer)
{
    throw new cRuntimeError(this,"netPack() not implemented");
}

void cBasicChannel::netUnpack(cCommBuffer *buffer)
{
    throw new cRuntimeError(this,"netUnpack() not implemented");
}

void cBasicChannel::rereadPars()
{
    flags &= ~FL_BASICCHANNELFLAGS;

    cPar& delay = par("delay");
    if (!delay.isVolatile() && delay.doubleValue()==0) flags |= FL_CONSTDELAY;
    //...FIXME todo!!! plus: call this from parameterChanged()! plus call this from initialize()!!
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
    return simulation.simTime()<transm_finishes;
}

bool cBasicChannel::deliver(cMessage *msg, simtime_t t)
{
    // if channel is disabled, signal that message should be deleted
    if (flags & FL_ISDISABLED)
        return false;
    if (!(flags & FL_CONSTDISABLED && par("disabled").boolValue()))
        return false;

    // must wait until previous transmissions end
    if (t < transm_finishes)
        t = transm_finishes;

    if (flags & FL_HASDATARATE)
    {
        double datarate = (flags & FL_CONSTDATARATE) ? dataratepar : par("datarate").doubleValue();
        if (datarate<0)
            throw new cRuntimeError(this, "negative datarate %g", datarate);
        if (datarate>0)
            t += (simtime_t) (msg->length() / datarate);
        transm_finishes = t;
    }

    // propagation delay modelling
    if (flags & FL_HASDELAY)
    {
        simtime_t delay = (flags & FL_CONSTDELAY) ? delaypar : par("delay").doubleValue();
        if (delay<0)
            throw new cRuntimeError(this,"negative delay %g", delay);
        t += delay;
    }

    // bit error rate modelling
    if (flags & FL_HASERROR)
    {
        double error = (flags & FL_CONSTERROR) ? errorpar : par("error").doubleValue();
        if (error<0 || error>1)
            throw new cRuntimeError(this,"wring bit error rate %g", error);
        if (dblrand() < 1.0 - pow(1.0-error, msg->length()))
            msg->setBitError(true);
    }

    // hand over msg to next gate
    return fromGate()->toGate()->deliver(msg, t);
}

