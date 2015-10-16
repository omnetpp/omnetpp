//
// This file is part of an OMNeT++/OMNEST simulation example.
//
// Copyright (C) 1992-2015 Andras Varga
//
// This file is distributed WITHOUT ANY WARRANTY. See the file
// `license' for details on this and other legal matters.
//

#include <omnetpp.h>
#include "TelnetPkt_m.h"

using namespace omnetpp;

/**
 * Simple model of a Telnet client.
 */
class TelnetClient : public cSimpleModule
{
  private:
    int addr;
    int srvAddr;

  protected:
    virtual void initialize() override;
    virtual void handleMessage(cMessage *msg) override;
    void simulateUserTyping();
    void processEcho(TelnetPkt *telnetPkt);
};

Define_Module(TelnetClient);

void TelnetClient::initialize()
{
    addr = par("addr");
    srvAddr = par("srvAddr");

    cMessage *timer = new cMessage("timer");
    scheduleAt(simTime()+par("sendIaTime").doubleValue(), timer);
}

void TelnetClient::handleMessage(cMessage *msg)
{
    if (msg->isSelfMessage()) {
        simulateUserTyping();
        scheduleAt(simTime()+par("sendIaTime").doubleValue(), msg);
    }
    else {
        processEcho(check_and_cast<TelnetPkt *>(msg));
    }
}

void TelnetClient::simulateUserTyping()
{
    // send telnet packet
    TelnetPkt *telnetPkt = new TelnetPkt("x");
    telnetPkt->setPayload("x");
    telnetPkt->setDestAddress(srvAddr);
    telnetPkt->setSrcAddress(addr);

    send(telnetPkt, "g$o");
}

void TelnetClient::processEcho(TelnetPkt *telnetPkt)
{
    delete telnetPkt;
}

