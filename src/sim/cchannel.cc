//========================================================================
//
//  CCHANNEL.CC - part of
//                         OMNeT++
//              Discrete System Simulation in C++
//
//   Member functions of
//    cChannel : channel class
//
//  Author: Andras Varga
//
//========================================================================

/*--------------------------------------------------------------*
  Copyright (C) 1992-2004 Andras Varga

  This file is distributed WITHOUT ANY WARRANTY. See the file
  `license' for details on this and other legal matters.
*--------------------------------------------------------------*/

#include "cmessage.h"
#include "cmodule.h"
#include "carray.h"
#include "cpar.h"
#include "cenvir.h"
#include "random.h"
#include "distrib.h"
#include "csimul.h"
#include "macros.h"
#include "cgate.h"
#include "cchannel.h"
#include "cexception.h"

#ifdef WITH_PARSIM
#include "ccommbuffer.h"
#endif

using std::ostream;

//=== registration
Register_Class(cChannel);
Register_Class(cSimpleChannel);

//=========================================================================
//=== cChannel - member functions

cChannel::cChannel(const cChannel& ch) : cObject()
{
    parlistp = NULL;
    channeltypep = NULL;
    fromgatep = NULL;

    setName( ch.name() );
    operator=( ch );
}

cChannel::cChannel(const char *name, cChannelType *chantype) : cObject(name)
{
    parlistp = NULL;
    fromgatep = NULL;
    channeltypep = chantype;
}

cChannel::~cChannel()
{
    dropAndDelete(parlistp);
}

std::string cChannel::info() const
{
    if (!parlistp)
        return std::string();

    // print all attributes
    std::stringstream out;
    for (int i=0; i<parlistp->items(); i++)
    {
        cObject *p = parlistp->get(i);
        if (!p) continue;
        out << p->fullName() << "=" << p->info() << " ";
    }
    return out.str();
}

void cChannel::forEach( ForeachFunc do_fn )
{
    if (do_fn(this,true))
        if (parlistp) parlistp->forEach( do_fn );
    do_fn(this,false);
}

void cChannel::writeContents(ostream& os)
{
    if (parlistp)
    {
        os << "  parameter list:\n";
        parlistp->writeContents( os );
    }
    else
    {
        os << "  no parameter list\n";
    }
}

void cChannel::netPack(cCommBuffer *buffer)
{
    throw new cException(this,"netPack() not implemented");
}

void cChannel::netUnpack(cCommBuffer *buffer)
{
    throw new cException(this,"netUnpack() not implemented");
}

cChannel& cChannel::operator=(const cChannel& ch)
{
    if (this==&ch) return *this;

    cObject::operator=(ch);

    fromgatep = NULL;

    dropAndDelete(parlistp);
    take(parlistp = (cArray *)ch.parlistp->dup());
    return *this;
}

void cChannel::_createparlist()
{
    parlistp = new cArray( "parameters", 5, 5 );
    take( parlistp );
}

cPar& cChannel::par(int n)
{
    cArray& parlist = parList();
    cPar *p = (cPar *)parlist[n];
    if (!p)
        throw new cException(this,"has no parameter #%d",n);
    return *p;
}

cPar& cChannel::par(const char *s)
{
    cArray& parlist = parList();
    cPar *p = (cPar *)parlist.get(s);
    if (!p)
        throw new cException(this,"has no parameter called `%s'",s);
    return *p;
}

cArray& cChannel::parList()
{
    if (!parlistp)
        _createparlist();
    return *parlistp;
}

int cChannel::findPar(const char *s) const
{
    if (!parlistp) return -1;
    return parlistp->find( s );
}

bool cChannel::deliver(cMessage *msg, simtime_t t)
{
    // hand over msg to next gate
    return fromGate()->toGate()->deliver(msg, t);
}

//=========================================================

cSimpleChannel::cSimpleChannel(const char *name) : cChannel(name)
{
    disabledp = errorp = delayp = dataratep = NULL;
    transm_finishes = 0.0;
}

cSimpleChannel::cSimpleChannel(const cSimpleChannel& ch) : cChannel()
{
    disabledp = errorp = delayp = dataratep = NULL;
    transm_finishes = 0.0;

    setName( ch.name() );
    operator=( ch );
}

cSimpleChannel::~cSimpleChannel()
{
}

std::string cSimpleChannel::info() const
{
    return cChannel::info();
    // append other info
    //TBD
}

void cSimpleChannel::forEach( ForeachFunc do_fn )
{
    if (do_fn(this,true))
    {
        if (parlistp) parlistp->forEach( do_fn );
    }
    do_fn(this,false);
}

void cSimpleChannel::writeContents(ostream& os)
{
    cChannel::writeContents( os );

    if (dataratep)
    {
        os << "  transmission state: " << (isBusy() ? "busy" : "free") << '\n';
        if (isBusy())
            os << "  transmission finishes: " << simtimeToStr(transm_finishes) << '\n';
    }
}

void cSimpleChannel::netPack(cCommBuffer *buffer)
{
    throw new cException(this,"netPack() not implemented");
}

void cSimpleChannel::netUnpack(cCommBuffer *buffer)
{
    throw new cException(this,"netUnpack() not implemented");
}

cSimpleChannel& cSimpleChannel::operator=(const cSimpleChannel& ch)
{
    if (this==&ch) return *this;

    cChannel::operator=(ch);

    cArray& parlist = _parList();
    disabledp = (cPar *)parlist.get("disabled");
    delayp = (cPar *)parlist.get("delay");
    errorp = (cPar *)parlist.get("error");
    dataratep = (cPar *)parlist.get("datarate");

    return *this;
}


void cSimpleChannel::setDelay(double d)
{
    if (!delayp)
    {
        delayp = new cPar("delay");
        _parList().set(delayp);
    }
    delayp->setDoubleValue(d);
}

void cSimpleChannel::setError(double d)
{
    if (!errorp)
    {
        errorp = new cPar("error");
        _parList().set(errorp);
    }
    errorp->setDoubleValue(d);
}

void cSimpleChannel::setDatarate(double d)
{
    if (!dataratep)
    {
        dataratep = new cPar("datarate");
        _parList().set(dataratep);
    }
    dataratep->setDoubleValue(d);
}

void cSimpleChannel::setDisabled(bool d)
{
    if (!disabledp)
    {
        disabledp = new cPar("disabled");
        _parList().set(disabledp);
    }
    disabledp->setBoolValue(d);
}

void cSimpleChannel::setDelay(cPar *p)
{
    if (!p)
    {
        delete _parList().remove("delay");
        delayp = NULL;
        return;
    }
    p->setName("delay");
    _parList().set(p);
    delayp = p;
}

void cSimpleChannel::setError(cPar *p)
{
    if (!p)
    {
        delete _parList().remove("error");
        errorp = NULL;
        return;
    }
    p->setName("error");
    _parList().set(p);
    errorp = p;
}

void cSimpleChannel::setDatarate(cPar *p)
{
    if (!p)
    {
        delete _parList().remove("datarate");
        dataratep = NULL;
        return;
    }
    p->setName("datarate");
    _parList().set(p);
    dataratep = p;
}

cPar& cSimpleChannel::addPar(const char *s)
{
    cPar *p = new cPar(s);
    _parList().set(p);
    if (!opp_strcmp(s,"disabled"))
        disabledp = p;
    else if (!opp_strcmp(s,"delay"))
        delayp = p;
    else if (!opp_strcmp(s,"error"))
        errorp = p;
    else if (!opp_strcmp(s,"datarate"))
        dataratep = p;

    return *p;
}

cPar& cSimpleChannel::addPar(cPar *p)
{
    _parList().set(p);
    const char *s = p->name();
    if (!opp_strcmp(s,"disabled"))
        disabledp = p;
    else if (!opp_strcmp(s,"delay"))
        delayp = p;
    else if (!opp_strcmp(s,"error"))
        errorp = p;
    else if (!opp_strcmp(s,"datarate"))
        dataratep = p;
    return *p;
}

bool cSimpleChannel::isBusy() const
{
    return simulation.simTime()<transm_finishes;
}

bool cSimpleChannel::deliver(cMessage *msg, simtime_t t)
{
    // if channel is disabled, signal that message should be deleted
    if (disabledp && (long)(*disabledp)!=0)
        return false;

    // transmission delay modelling
    double datarate;

    if (dataratep && (datarate = (double)(*dataratep))!=0.0)
    {
        if (t < transm_finishes)     // must wait until previous
            t = transm_finishes;     //   transmissions end
        t += (simtime_t) (msg->length() / datarate);
        transm_finishes = t;
    }

    // propagation delay modelling
    if (delayp)
    {
        simtime_t delay = (*delayp);
        if (delay<0)
            throw new cException(this,"negative delay %g",delay);
        t += delay;
    }

    // bit error rate modelling
    if (errorp)
    {
        if( dblrand() < 1.0 - pow(1.0-(double)(*errorp), msg->length()) )
            msg->setBitError(true);
    }

    // hand over msg to next gate
    return fromGate()->toGate()->deliver(msg, t);
}

