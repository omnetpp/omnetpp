//========================================================================
//  CGATE.CC - part of
//
//                 OMNeT++/OMNEST
//              Discrete System Simulation in C++
//
//   Member functions of
//    cGate      : a module gate
//
//  Author: Andras Varga
//
//========================================================================

/*--------------------------------------------------------------*
  Copyright (C) 1992-2005 Andras Varga

  This file is distributed WITHOUT ANY WARRANTY. See the file
  `license' for details on this and other legal matters.
*--------------------------------------------------------------*/

#include <math.h>            // pow
#include <stdio.h>           // sprintf
#include <string.h>          // strcpy
#include "cmessage.h"
#include "cmodule.h"
#include "carray.h"
#include "cenvir.h"
#include "csimulation.h"
#include "globals.h"
#include "cgate.h"
#include "cchannel.h"
#include "cbasicchannel.h"
#include "cdisplaystring.h"
#include "util.h"
#include "ccomponenttype.h"

using std::ostream;

//=========================================================================
//=== cGate -- member functions

cGate::cGate(const cGate& gate) : cObject()
{
    fullname = NULL;

    channelp = NULL;

    dispstr = NULL;

    setName(gate.name());
    operator=(gate);
}

cGate::cGate(const char *s, char tp) : cObject(s)
{
    fullname = NULL;

    fromgatep = togatep = NULL;
    gatetype() = tp;

    serno = 0;
    vectsize = -1;  // not a vector

    omodp = NULL; gateid = -1; // to be set later
    channelp = NULL;

    dispstr = NULL;
}

cGate::~cGate()
{
    dropAndDelete(channelp);
    delete dispstr;
    delete [] fullname;
}

void cGate::forEachChild(cVisitor *v)
{
    if (channelp)
        v->visit(channelp);
}

cGate& cGate::operator=(const cGate& gate)
{
    throw new cRuntimeError(this, eCANTDUP);
}

void cGate::setName(const char *s)
{
    cObject::setName(s);

    // invalidate fullname (it'll be recreated on demand)
    if (fullname)
    {
        delete [] fullname;
        fullname = NULL;
    }
}

const char *cGate::fullName() const
{
    // if not in a vector, normal name() will do
    if (!isVector())
        return name();

    // otherwise, produce fullname if not already there
    if (!fullname)
    {
        fullname = new char[opp_strlen(name())+10];
        strcpy(fullname, name());
        opp_appendindex(fullname, index());
    }
    return fullname;
}

std::string cGate::fullPath() const
{
    // TBD make use of cModule's cached fullPath()
    return std::string(fullPath(fullpathbuf,MAX_OBJECTFULLPATH));
}

const char *cGate::fullPath(char *buffer, int bufsize) const
{
    // check we got a decent buffer
    if (!buffer || bufsize<4)
    {
        if (buffer) buffer[0]='\0';
        return "(fullPath(): no buffer or buffer too small)";
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
    os << "  type:  " <<  (gatetype()==INPUT ? "input" : gatetype()==OUTPUT ? "output" : "inout") << '\n';
    cGate *inside = (gatetype()==INPUT) ? togatep : fromgatep; //FIXME for inout???
    cGate *outside = (gatetype()==INPUT) ? fromgatep : togatep; //FIXME for inout???
    os << "  inside connection:  " << (inside?inside->fullPath().c_str():"(not connected)") << '\n';
    os << "  outside connection: " << (outside?outside->fullPath().c_str():"(not connected)") << '\n';
}

std::string cGate::info() const
{
    const char *arrow;
    cGate const * g;
    cGate const * conng;

    if (gatetype()==OUTPUT)
        {arrow = "--> "; g = togatep; conng = this;}
    else if (gatetype()==INPUT)
        {arrow = "<-- "; g = fromgatep; conng = fromgatep;}
    else // INOUT
        {arrow = "<--> "; g = fromgatep; conng = fromgatep;}

    // append useful info to buf
    if (vectsize==0)
        return std::string("(placeholder for zero-size vector)");
    if (!g)
        return std::string("not connected");

    std::stringstream out;
    out << arrow;

    if (channelp)
        out << channelp->info() << arrow;

    out << (g->ownerModule()==ownerModule()->parentModule() ? "<parent>" : g->ownerModule()->fullName());
    out << "." << g->fullName();
    return out.str();
}

void cGate::setOwnerModule(cModule *m, int gid)
{
    omodp = m;
    gateid = gid;
}

void cGate::setIndex(int sn, int vs)
{
    serno = sn;
    vectsize = vs;

    // invalidate fullname (it'll be recreated on demand)
    if (fullname)
    {
        delete [] fullname;
        fullname = NULL;
    }
}

void cGate::connectTo(cGate *g, cChannel *chan)
{
    if (vectsize==0)
        throw new cRuntimeError(this, "connectTo(): gate vector size is zero");
    if (togatep)
        throw new cRuntimeError(this, "connectTo(): gate already connected");
    if (!g)
        throw new cRuntimeError(this, "connectTo(): destination gate cannot be NULL pointer");
    if (g->vectsize==0)
        throw new cRuntimeError(this, "connectTo(): destination gate vector size is zero");
    if (g->fromgatep)
        throw new cRuntimeError(this, "connectTo(): destination gate already connected");

    // build new connection
    togatep = g;
    togatep->fromgatep = this;
    if (chan)
        setChannel(chan);

    ev.connectionCreated(this);
}

void cGate::disconnect()
{
    if (!togatep) return;

    // notify envir that old conn gets removed
    ev.connectionRemoved(this);

    // remove connection
    togatep->fromgatep = NULL;
    togatep = NULL;

    // and channel object
    dropAndDelete(channelp);
    channelp = NULL;
}

void cGate::setChannel(cChannel *ch)
{
    if (ch == channelp)
        return;

    dropAndDelete(channelp);
    channelp = ch;
    if (channelp)
    {
        channelp->setFromGate(this);
        take(channelp);
    }
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

bool cGate::deliver(cMessage *msg, simtime_t t)
{
    if (togatep==NULL)
    {
        ownerModule()->arrived(msg, id(), t);
        return true;
    }
    else
    {
        if (channelp)
            return channelp->deliver(msg, t);
        else
            return togatep->deliver(msg, t);
    }
}

bool cGate::isBusy() const
{
    cBasicChannel *ch = dynamic_cast<cBasicChannel *>(channelp);
    return ch ? ch->isBusy() : false;
}

simtime_t cGate::transmissionFinishes() const
{
    cBasicChannel *ch = dynamic_cast<cBasicChannel *>(channelp);
    return ch ? ch->transmissionFinishes() : 0.0;
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
    if (type()==INPUT)
        return fromgatep!=NULL;
    else
        return togatep!=NULL;
}

bool cGate::isConnectedInside() const
{
    if (type()==INPUT)
        return togatep!=NULL;
    else
        return fromgatep!=NULL;
}

bool cGate::isConnected() const
{
    // for compound modules, both inside and outside must be non-NULL,
    // for simple modules, only check outside.
    if (!ownerModule()->isSimple())
        return fromgatep!=NULL && togatep!=NULL;
    else
        return isConnectedOutside();
}

bool cGate::isRouteOK() const
{
    return sourceGate()->ownerModule()->isSimple() &&
           destinationGate()->ownerModule()->isSimple();
}

cDisplayString& cGate::displayString()
{
    if (!dispstr)
    {
        dispstr = new cDisplayString();
        dispstr->setRoleToConnection(this);
    }
    return *dispstr;
}

// DEPRECATED
void cGate::setDisplayString(const char *s, bool immediate)
{
    displayString().parse(s);
}


