//
// This file is part of an OMNeT++/OMNEST simulation example.
//
// Copyright (C) 1992-2015 Andras Varga
//
// This file is distributed WITHOUT ANY WARRANTY. See the file
// `license' for details on this and other legal matters.
//

#include <omnetpp.h>
#include "NetPkt_m.h"

using namespace omnetpp;

/**
 * Represents the network "cloud" between clients and the server;
 * see NED file for more info.
 */
class Cloud : public cSimpleModule
{
  private:
    simtime_t propDelay;

  protected:
    virtual void initialize() override;
    virtual void handleMessage(cMessage *msg) override;
};

Define_Module(Cloud);

void Cloud::initialize()
{
    propDelay = (double)par("propDelay");
}

void Cloud::handleMessage(cMessage *msg)
{
    // determine destination address
    NetPkt *pkt = check_and_cast<NetPkt *>(msg);
    int dest = pkt->getDestAddress();
    EV << "Relaying packet to addr=" << dest << endl;

    // send msg to destination after the delay
    sendDelayed(pkt, propDelay, "g$o", dest);
}

