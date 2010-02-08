//
// This file is part of an OMNeT++/OMNEST simulation example.
//
// Copyright (C) 1992-2008 Andras Varga
//
// This file is distributed WITHOUT ANY WARRANTY. See the file
// `license' for details on this and other legal matters.
//

#include <omnetpp.h>

namespace fifo {

/**
 * Packet sink; see NED file for more info.
 */
class Sink : public cSimpleModule
{
  private:
    simsignal_t lifetimeSignal;

  protected:
    virtual void initialize();
    virtual void handleMessage(cMessage *msg);
};

Define_Module( Sink );

void Sink::initialize()
{
    lifetimeSignal = registerSignal("lifetime");
}

void Sink::handleMessage(cMessage *msg)
{
    simtime_t lifetime = simTime() - msg->getCreationTime();
    EV << "Received " << msg->getName() << ", lifetime: " << lifetime << "s" << endl;
    emit(lifetimeSignal, lifetime);
    delete msg;
}

}; //namespace

