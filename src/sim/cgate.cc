//========================================================================
//
//  CGATE.CC - part of
//                         OMNeT++
//              Discrete System Simulation in C++
//
//   Member functions of
//    cGate      : a module gate
//
//  Author: Andras Varga
//
//========================================================================

/*--------------------------------------------------------------*
  Copyright (C) 1992-2003 Andras Varga

  This file is distributed WITHOUT ANY WARRANTY. See the file
  `license' for details on this and other legal matters.
*--------------------------------------------------------------*/

#include <math.h>            // pow
#include <stdio.h>           // sprintf
#include <string.h>          // strcpy
#include "cmessage.h"
#include "cmodule.h"
#include "carray.h"
#include "cpar.h"
#include "cenvir.h"
#include "csimul.h"
#include "macros.h"
#include "cgate.h"
#include "cchannel.h"

//=========================================================================
//=== cGate -- member functions

cGate::cGate(const cGate& gate) : cObject()
{
    fullname = NULL;

    channelp = NULL;

    notify_inspector = NULL;
    data_for_inspector = NULL;

    setName(gate.name());
    operator=(gate);
}

cGate::cGate(const char *s, char tp) : cObject(s)
{
    fullname = NULL;

    fromgatep = togatep = NULL;
    typ = tp;

    serno = 0;
    vectsize = -1;  // not a vector

    omodp = NULL; gateid = -1; // to be set later

    channelp = NULL;

    notify_inspector = NULL;
    data_for_inspector = NULL;

    takeOwnership( false );
}

cGate::~cGate()
{
    delete [] fullname;
}

void cGate::forEach(ForeachFunc do_fn)
{
    if (do_fn(this,true))
    {
       if (channelp)  channelp->forEach( do_fn );
    }
    do_fn(this,false);
}

cGate& cGate::operator=(const cGate& gate)
{
    if (this==&gate) return *this;

    cObject::operator=(gate);

    omodp = NULL; gateid = -1;        // to be set later

    fromgatep = gate.fromgatep;
    togatep = gate.togatep;

    typ = gate.typ;
    serno = gate.serno;
    vectsize = gate.vectsize;

    if (channelp && channelp->owner()==const_cast<cGate*>(&gate))
        discard(channelp);
    channelp = gate.channelp;
    if (channelp->owner()==const_cast<cGate*>(&gate))
        channelp = (cChannel *)channelp->dup();

    setDisplayString( gate.displayString() );

    return *this;
}

const char *cGate::fullName() const
{
    // if not in a vector, normal name() will do
    if (!isVector())
       return name();

    // produce index with name here (lazy solution: produce name in each call,
    // instead of overriding both setName() and setIndex()...)
    if (fullname)  delete [] fullname;
    fullname = new char[opp_strlen(name())+10];
    sprintf(fullname, "%s[%d]", name(), index() );
    return fullname;
}

const char *cGate::fullPath() const
{
    return fullPath(fullpathbuf,FULLPATHBUF_SIZE);
}

const char *cGate::fullPath(char *buffer, int bufsize) const
{
    // check we got a decent buffer
    if (!buffer || bufsize<4)
    {
        if (buffer) buffer[0]='\0';
        return "(fullPath(): no or too small buffer)";
    }

    // hide gate vector: skip directly to owner module
    // append parent path + "."
    char *buf = buffer;
    if (omodp!=NULL)
    {
        omodp->fullPath(buf,bufsize);
        int len = strlen(buf);
        buf+=len;
        bufsize-=len;
        *buf++ = '.';
        bufsize--;
    }

    // append our own name
    opp_strprettytrunc(buf, fullName(), bufsize-1);
    return buffer;
}

void cGate::writeContents(ostream& os)
{
    os << "  type:  " <<  (typ=='I' ? "input" : "output") << '\n';
    cGate *inside = (typ=='I') ? togatep : fromgatep;
    cGate *outside = (typ=='I') ? fromgatep : togatep;
    os << "  inside connection:  " << (inside?inside->fullPath():"(not connected)") << '\n';
    os << "  outside connection: " << (outside?outside->fullPath():"(not connected)") << '\n';
}

void cGate::info(char *buf)
{
    // info() string will be like:
    //    "(cGate) outgate  --> <parent>.outgate  DE" (DER:DelayErrorDatarate)
    cObject::info( buf );

    // find end of string
    char *b = buf;
    while(*b) b++;

    char channel[5], *arrow, *s;
    cGate *g, *conng;

    if (typ=='O')
        {arrow = "--> "; g = togatep; conng = this;}
    else
        {arrow = "<-- "; g = fromgatep; conng = fromgatep;}

    s = channel;
    if (conng && conng->delay()) *s++='D';
    if (conng && conng->error()) *s++='E';
    if (conng && conng->datarate()) *s++='R';
    *s++ = ' ';
    *s = '\0';

    // append useful info to buf
    if (!g) {
        strcpy(b,"  "); b+=2;
        strcpy(b,arrow); b+=4;
        strcpy(b," (not connected)");
    }
    else
    {
        strcpy(b,"  "); b+=2;
        strcpy(b,arrow); b+=4;
        if (channel[0]!=' ')
           {strcpy(b,channel);strcat(b,arrow);while(*b) b++;}
        strcpy(b, g->ownerModule()==ownerModule()->parentModule() ?
                  "<parent>" : g->ownerModule()->fullName() );
        while(*b) b++; *b++ = '.';
        strcpy(b, g->fullName() );
    }
}

void cGate::setIndex(int sn, int vs)
{
    serno = sn;
    vectsize = vs;
}

void cGate::connectTo(cGate *g, cChannel *chan)
{
    // break old connection
    g->fromgatep = NULL;

    // build new connection
    togatep = g;
    togatep->fromgatep = this;

    // replace old channel (or delete it if chan==NULL)
    setChannel(chan);
}

void cGate::setFrom(cGate *g)
{
    fromgatep = g;
}

void cGate::setTo(cGate *g)
{
    togatep = g;
}

void cGate::setOwnerModule(cModule *m, int gid)
{
    omodp = m;
    gateid = gid;
}

void cGate::setLink(cLinkType *lnk)
{
    setChannel(new cSimpleChannel("channel",lnk));
}

void cGate::setChannel(cChannel *ch)
{
    if (ch == channelp)
        return;

    if (channelp && channelp->owner()==this)
        discard(channelp);

    channelp = ch;
    if (channelp)
    {
        channelp->setFromGate(this);
        if (takeOwnership())
            take(channelp);
    }
}

void cGate::setDelay(cPar *p)
{
    if (!channelp)
        setChannel(new cSimpleChannel("channel"));
    // FIXME: check that channelp really points to a cSimpleChannel!
    ((cSimpleChannel *)channelp)->setDelay(p);
}

void cGate::setError(cPar *p)
{
    if (!channelp)
        setChannel(new cSimpleChannel("channel"));
    // FIXME: check that channelp really points to a cSimpleChannel!
    ((cSimpleChannel *)channelp)->setError(p);
}

void cGate::setDataRate(cPar *p)
{
    if (!channelp)
        setChannel(new cSimpleChannel("channel"));
    // FIXME: check that channelp really points to a cSimpleChannel!
    ((cSimpleChannel *)channelp)->setDatarate(p);
}

cLinkType *cGate::link() const
{
    return channelp ? channelp->link() : NULL;
}

cPar *cGate::delay() const
{
    // FIXME: check that channelp really points to a cSimpleChannel!
    return channelp ? ((cSimpleChannel *)channelp)->delay() : NULL;
}

cPar *cGate::error() const
{
    // FIXME: check that channelp really points to a cSimpleChannel!
    return channelp ? ((cSimpleChannel *)channelp)->error() : NULL;
}

cPar *cGate::datarate() const
{
    // FIXME: check that channelp really points to a cSimpleChannel!
    return channelp ? ((cSimpleChannel *)channelp)->datarate() : NULL;
}

cGate *cGate::sourceGate() const
{
    const cGate *g;
    for (g=this; g->fromgatep!=NULL; g=g->fromgatep);
    return const_cast<cGate *>(g);
}

cGate *cGate::destinationGate() const
{
    const cGate *g;
    for (g=this; g->togatep!=NULL; g=g->togatep);
    return const_cast<cGate *>(g);
}

void cGate::deliver(cMessage *msg, simtime_t t)
{
    if (togatep==NULL)
    {
        ownerModule()->arrived(msg, id(), t);
    }
    else
    {
        if (channelp)
            channelp->deliver(msg, t);
        else
            togatep->deliver(msg, t);
    }
}

bool cGate::isBusy() const
{
    // FIXME: check that channelp really points to a cSimpleChannel!
    return channelp ? ((cSimpleChannel *)channelp)->isBusy() : false;
}

simtime_t cGate::transmissionFinishes() const
{
    // FIXME: check that channelp really points to a cSimpleChannel!
    return channelp ? ((cSimpleChannel *)channelp)->transmissionFinishes() : simulation.simTime();
}

int cGate::routeContains( cModule *mod, int gate )
{
    cGate *g;

    for (g=this; g!=NULL; g=g->fromgatep)
        if( g->ownerModule()==mod && (gate==-1 || g->id()==gate) )
            return 1;
    for (g=togatep; g!=NULL; g=g->togatep)
        if( g->ownerModule()==mod && (gate==-1 || g->id()==gate) )
            return 1;
    return 0;
}

bool cGate::isConnectedOutside() const
{
    if (type()=='I')
        return fromgatep!=NULL;
    else
        return togatep!=NULL;
}

bool cGate::isConnectedInside() const
{
    if (type()=='I')
        return togatep!=NULL;
    else
        return fromgatep!=NULL;
}

bool cGate::isConnected() const
{
    // for compound modules, both inside and outside must be non-NULL,
    // for simple modules, only check outside.
    if (!ownerModule()->isSimple() && ownerModule()->isOnLocalMachine())
        return fromgatep!=NULL && togatep!=NULL;
    else
        return isConnectedOutside();
}

bool cGate::isRouteOK() const
{
    return sourceGate()->ownerModule()->isSimple() &&
           destinationGate()->ownerModule()->isSimple();
}

const char *cGate::displayString() const
{
    if ((const char *)dispstr != NULL)
        return dispstr;

    // no hardcoded display string -- try to get it from Envir
    // Note: connection display strings are stored in the connection's source gate
    char dispname[128];
    if (type()=='O')
    {
        cModule *parent = ownerModule()->parentModule();
        if (!parent) return "";
        sprintf(dispname, "%s.%s.%s",parent->className(),ownerModule()->fullName(), fullName());
    }
    else
    {
        sprintf(dispname, "%s.%s",ownerModule()->className(),fullName());
    }
    const char *s = ev.getDisplayString(simulation.runNumber(),dispname);
    return s ? s : "";
}

void cGate::setDisplayString(const char *s, bool immediate)
{
    dispstr = s;

    // notify this gate's inspector
    if (notify_inspector)
        notify_inspector(this,immediate,data_for_inspector);

    // notify compound module where the connection (whose source is this gate) is displayed
    if (!toGate())  return; // if not connected, nothing to worry

    cModule *notifymodule = NULL;
    if (type()=='O')
        notifymodule = ownerModule()->parentModule();
    else
        notifymodule = ownerModule();

    if (notifymodule && notifymodule->notify_inspector)
        notifymodule->notify_inspector(notifymodule,immediate,notifymodule->data_for_inspector);
}

void cGate::setDisplayStringNotify(void (*notify_func)(cGate*,bool,void*), void *data)
{
    notify_inspector = notify_func;
    data_for_inspector = data;
}
