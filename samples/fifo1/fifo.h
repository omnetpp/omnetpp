//-------------------------------------------------------------
// file: fifo.h
//        (part of Fifo1 - an OMNeT++ demo simulation)
//-------------------------------------------------------------

#ifndef __FIFO_H
#define __FIFO_H

// FF1AbstractFifo : abstract base class for single-server queues
class FF1AbstractFifo : public cSimpleModule
{
  public:
    Module_Class_Members(FF1AbstractFifo,cSimpleModule,16384);

    cMessage *msgServiced;
    cMessage *endServiceMsg;
    cQueue queue;

    virtual void activity();
    virtual void finish();

    // hook functions to (re)define behaviour
    virtual void arrival(cMessage *msg) {}
    virtual simtime_t startService(cMessage *msg) = 0;
    virtual void endService(cMessage *msg) = 0;
};

// ACPFifo : single-server queue with given service time
class ACPFifo : public FF1AbstractFifo
{
  public:
    Module_Class_Members(ACPFifo,FF1AbstractFifo,16384);

    virtual simtime_t startService(cMessage *msg);
    virtual void endService(cMessage *msg);
};

// ACBFifo : single-server queue with service time based on message length
class ACBFifo : public FF1AbstractFifo
{
  public:
    Module_Class_Members(ACBFifo,FF1AbstractFifo,16384);

    virtual simtime_t startService(cMessage *msg);
    virtual void endService(cMessage *msg);
};

#endif
