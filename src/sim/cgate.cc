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
  Copyright (C) 1992-2001 Andras Varga
  Technical University of Budapest, Dept. of Telecommunications,
  Stoczek u.2, H-1111 Budapest, Hungary.

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

//=========================================================================
//=== cGate -- member functions

cGate::cGate(_CONST cGate& gate) : cObject()
{
    linkp = NULL;
    delayp = errorp = dataratep = NULL;

    transm_finishes = 0.0;

    notify_inspector = NULL;
    data_for_inspector = NULL;

    setName(gate.name());
    operator=(gate);
}

cGate::cGate(const char *s, char tp) : cObject(s)
{
    fromgatep = togatep = NULL;
    typ = tp;

    serno = 0;
    vectsize = -1;  // not a vector

    omodp = NULL; gateid = -1; // to be set later

    linkp = NULL;
    delayp = errorp = dataratep = NULL;

    transm_finishes = 0.0;

    notify_inspector = NULL;
    data_for_inspector = NULL;

    takeOwnership( false );
}

void cGate::forEach(ForeachFunc do_fn)
{
    if (do_fn(this,true))
    {
       if (delayp)    delayp->forEach( do_fn );
       if (errorp)    errorp->forEach( do_fn );
       if (dataratep) dataratep->forEach( do_fn );
    }
    do_fn(this,false);
}

cGate& cGate::operator=(_CONST cGate& gate)
{
   if (this==&gate) return *this;

   cObject::operator=(gate);

   omodp = NULL; gateid = -1;        // to be set later

   fromgatep = gate.fromgatep;
   togatep = gate.togatep;

   typ = gate.typ;
   serno = gate.serno;
   vectsize = gate.vectsize;

   linkp = gate.linkp;  // gates never own link objects, just point to them
   delayp = gate.delayp;
   if (delayp->owner()==CONSTCAST(cGate*,&gate)) delayp = (cPar *)delayp->dup();
   errorp = gate.errorp;
   if (errorp->owner()==CONSTCAST(cGate*,&gate)) errorp = (cPar *)errorp->dup();
   dataratep = gate.dataratep;
   if (dataratep->owner()==CONSTCAST(cGate*,&gate)) dataratep= (cPar *)dataratep->dup();

   setDisplayString( gate.displayString() );

   return *this;
}

const char *cGate::fullName() const
{
      static char buf[256];
      if (!isVector())
         return name();
      else {
         sprintf(buf, "%s[%d]", name(), index() );
         return buf;
      }
}

const char *cGate::fullPath() const
{
      // use cObject::fullPath()'s static buffer
      // hide gate vector: skip directly to owner module
      if (omodp!=NULL)
      {
         static char buf[512]; // should be big enough because there's no check!!
         const char *p = omodp->fullPath();
         if (p!=buf) strcpy(buf,p);
         strcat(buf,".");
         strcat(buf,fullName());
         return buf;
      }
      else
         return fullName();
}

void cGate::writeContents(ostream& os)
{
      char buf[81];

      os << "  type:  " <<  (typ=='I' ? "input" : "output") << '\n';
      cGate *inside = (typ=='I') ? togatep : fromgatep;
      cGate *outside = (typ=='I') ? fromgatep : togatep;
      os << "  inside connection:  " << (inside?inside->fullPath():"(not connected)") << '\n';
      os << "  outside connection: " << (outside?outside->fullPath():"(not connected)") << '\n';

      os << "  delay: ";
      if (delayp)
          {delayp->info(buf); os << buf << '\n';}
      else
          os << "-\n";

      os << "  error: ";
      if (errorp)
          {errorp->info(buf); os << buf << '\n';}
      else
          os << "-\n";

      os << "  data rate: ";
      if (dataratep)
          {dataratep->info(buf); os << buf << '\n';}
      else
          os << "-\n";

      if (dataratep)
      {
          os << "  transmission state: " << (isBusy() ? "busy" : "free") << '\n';
          if (isBusy())
              os << "  transmission finishes: " << simtimeToStr(transm_finishes) << '\n';
      }
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
      if (conng && conng->delayp) *s++='D';
      if (conng && conng->errorp) *s++='E';
      if (conng && conng->dataratep) *s++='R';
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
      linkp = lnk;
      bool tk = takeOwnership(); takeOwnership(true);
      setDelay( linkp ? linkp->createDelay() : NO(cPar) );
      setError( linkp ? linkp->createError() : NO(cPar) );
      setDataRate( linkp ? linkp->createDataRate() : NO(cPar) );
      takeOwnership(tk);
}

void cGate::setDelay(cPar *p)
{
      if (delayp && delayp->owner()==this)  free( delayp );
      delayp = p;
      if (delayp && takeOwnership()) take( delayp );
}

void cGate::setError(cPar *p)
{
      if (errorp && errorp->owner()==this)  free( errorp );
      errorp = p;
      if (errorp && takeOwnership()) take( errorp );
}

void cGate::setDataRate(cPar *p)
{
      if (dataratep && dataratep->owner()==this)  free( dataratep );
      dataratep = p;
      if (dataratep && takeOwnership()) take( dataratep );
}

cGate *cGate::sourceGate() _CONST
{
    _CONST cGate *g;
    for (g=this; g->fromgatep!=NULL; g=g->fromgatep);
    return CONSTCAST(cGate *,g);
}

cGate *cGate::destinationGate() _CONST
{
    _CONST cGate *g;
    for (g=this; g->togatep!=NULL; g=g->togatep);
    return CONSTCAST(cGate *,g);
}

void cGate::deliver(cMessage *msg)
{
    if (togatep==NULL)
    {
        ownerModule()->arrived( msg, id() );
    }
    else
    {
        // NOTE: prop. delay calculation should be _after_ transm. delay modeling

        // Transmission delay modelling
        double datarate;
        if (dataratep && (datarate = (double)(*dataratep))!=0.0)
        {
            if (msg->delivd < transm_finishes)     // must wait until previous
                   msg->delivd = transm_finishes;  //   transmissions end
            msg->delivd += (simtime_t) (msg->length() / datarate);
            transm_finishes = msg->delivd;
        }

        // Propagation delay modelling
        if (delayp)
            msg->delivd += (simtime_t) (*delayp);

        // Bit error rate modelling
        if (errorp)
            if( dblrand() < 1.0 - pow(1.0-(double)(*errorp), msg->length()) )
                msg->error=true;

        togatep->deliver( msg );
    }
}

bool cGate::isBusy() _CONST
{
    return simulation.simTime()<transm_finishes;
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

bool cGate::isConnected() const
{
    if (!ownerModule()->isSimple() && ownerModule()->isOnLocalMachine())
        return fromgatep!=NULL && togatep!=NULL;
    else if (type()=='I')
        return fromgatep!=NULL;
    else
        return togatep!=NULL;
}

bool cGate::isRouteOK() _CONST
{
    return sourceGate()->ownerModule()->isSimple() &&
           destinationGate()->ownerModule()->isSimple();
}

const char *cGate::displayString() _CONST
{
    if ((const char *)dispstr != NULL)
        return dispstr;

    // no hardcoded display string -- try to get it from Envir
    char dispname[128];

    // connection display strings are stored in the connection's "from" gate

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

void cGate::setDisplayString(const char *s)
{
    dispstr = s;
    if (notify_inspector) notify_inspector(data_for_inspector);
}

void cGate::setDisplayStringNotify(void (*notify_func)(void*), void *data)
{
    notify_inspector = notify_func;
    data_for_inspector = data;
}
