//==========================================================================
//   CGATE.H  -  header for
//                             OMNeT++
//            Discrete System Simulation in C++
//
//
//  Declaration of the following classes:
//    cGate       : module gate
//
//==========================================================================

/*--------------------------------------------------------------*
  Copyright (C) 1992,99 Andras Varga
  Technical University of Budapest, Dept. of Telecommunications,
  Stoczek u.2, H-1111 Budapest, Hungary.

  This file is distributed WITHOUT ANY WARRANTY. See the file
  `license' for details on this and other legal matters.
*--------------------------------------------------------------*/

#ifndef __CGATE_H
#define __CGATE_H

#include "cobject.h"

//=== classes mentioned here:
class  cGate;
class  cModule;
class  cPar;
class  cMessage;
class  cLinkType;

//==========================================================================
// cGate - represents a gate

class cGate : public cObject
{
        friend class cModule;
    protected:
        int  serno;         // index if gate vector, 0 otherwise
        int  vectsize;      // gate vector size (-1 if not vector)
        char typ;           // type of gate: 'I' or 'O'

        cLinkType *linkp;   // link prototype or NULL
        cPar *delayp;       // propagation delay or NULL
        cPar *errorp;       // bit error rate or NULL
        cPar *dataratep;    // data rate or NULL

        cModule *omodp;     // owner module
        int gateid;         // gate number within the module

        cGate *fromgatep;   // previous and next gate
        cGate *togatep;     //   in the route

        simtime_t transm_finishes; // end of transmission; used if dataratep!=NULL

        char *dispstr;      // the display string
        void (*notify_inspector)(void*); // to notify inspector about disp str changes
        void *data_for_inspector;

    public:
        cGate(cGate& gate);
        cGate(char *s, char tp);
        virtual ~cGate() {}

        // redefined functions
        virtual char *isA()  {return "cGate";}
        virtual cObject *dup()  {return new cGate(*this);}
        virtual char *inspectorFactoryName() {return "cGateIFC";}
        virtual void forEach(ForeachFunc f);
        virtual void info(char *buf);
        cGate& operator=(cGate& gate);
        virtual char *fullName();
        virtual char *fullPath();
        virtual void writeContents(ostream& os);

        // new functions
        void setIndex(int sn, int vs);
        void setLink(cLinkType *l);
        void setDelay(cPar *p);
        void setError(cPar *p);
        void setDataRate(cPar *p);
        void setFrom(cGate *g);
        void setTo(cGate *g);
        void setOwnerModule(cModule *m, int gid);

        bool isVector()        {return vectsize>=0;}
        int index()            {return serno;}
        int size()             {return vectsize<0?1:vectsize;}
        char type()            {return typ;}
        cModule *ownerModule() {return omodp;}
        int id()               {return gateid;}
        cLinkType *link()      {return linkp;}
        cPar *delay()          {return delayp;}
        cPar *error()          {return errorp;}
        cPar *datarate()       {return dataratep;}

        cGate *fromGate()      {return fromgatep;}
        cGate *toGate()        {return togatep;}

        cGate *sourceGate();
        cGate *destinationGate();

        bool isBusy();
        simtime_t transmissionFinishes() {return transm_finishes;}

        void deliver(cMessage *msg);   // called by send() functions

        int routeContains(cModule *m, int g=-1);
        bool isConnected();
        bool isRouteOK();

        // visualization/animation support
        void setDisplayString(char *s);
        char *displayString();
        void setDisplayStringNotify(void (*notify_func)(void*), void *data);
};

#endif

