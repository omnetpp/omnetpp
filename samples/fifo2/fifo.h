//-------------------------------------------------------------
// file: fifo.h
//        (part of Fifo2 - an OMNeT++ demo simulation)
//-------------------------------------------------------------

#ifndef __FIFO_H
#define __FIFO_H

// FF2AbstractFifo : abstract base class for single-server queues
class FF2AbstractFifo : public cSimpleModule
{
  public:
    Module_Class_Members(FF2AbstractFifo,cSimpleModule,0);

    cMessage *msgServiced;
    cMessage *endServiceMsg;
    cQueue queue;

    virtual void initialize();
    virtual void handleMessage(cMessage *msg);

    // hook functions to (re)define behaviour
    virtual void arrival(cMessage *msg) {}
    virtual simtime_t startService(cMessage *msg) = 0;
    virtual void endService(cMessage *msg) = 0;
};

// FF2PacketFifo : single-server queue with given service time
class FF2PacketFifo : public FF2AbstractFifo
{
  public:
    Module_Class_Members(FF2PacketFifo,FF2AbstractFifo,0);

    virtual simtime_t startService(cMessage *msg);
    virtual void endService(cMessage *msg);
};

// FF2BitFifo : single-server queue with service time based on message length
class FF2BitFifo : public FF2AbstractFifo
{
  public:
    Module_Class_Members(FF2BitFifo,FF2AbstractFifo,0);

    virtual simtime_t startService(cMessage *msg);
    virtual void endService(cMessage *msg);
};

#endif
