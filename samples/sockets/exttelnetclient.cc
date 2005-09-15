//
// This file is part of an OMNeT++/OMNEST simulation example.
//
// Copyright (C) 1992-2005 Andras Varga
//
// This file is distributed WITHOUT ANY WARRANTY. See the file
// `license' for details on this and other legal matters.
//


#include <omnetpp.h>
#include "telnetpkt_m.h"
#include "csocketrtscheduler.h"


/**
 * Simple model of a Telnet client.
 */
class ExtTelnetClient : public cSimpleModule
{
  private:
    cMessage *rtEvent;
    cSocketRTScheduler *rtScheduler;

    char recvBuffer[4000];
    int numRecvBytes;

    int addr;
    int srvAddr;

  public:
    ExtTelnetClient();
    virtual ~ExtTelnetClient();

  protected:
    virtual void initialize();
    virtual void handleMessage(cMessage *msg);
    void handleSocketEvent();
    void handleReply(TelnetPkt *telnetReply);
};

Define_Module(ExtTelnetClient);

ExtTelnetClient::ExtTelnetClient()
{
    rtEvent = NULL;
}

ExtTelnetClient::~ExtTelnetClient()
{
    cancelAndDelete(rtEvent);
}

void ExtTelnetClient::initialize()
{
    rtEvent = new cMessage("rtEvent");
    rtScheduler = check_and_cast<cSocketRTScheduler *>(simulation.scheduler());
    rtScheduler->setInterfaceModule(this, rtEvent, recvBuffer, 4000, &numRecvBytes);

    addr = par("addr");
    srvAddr = par("srvAddr");
}

void ExtTelnetClient::handleMessage(cMessage *msg)
{
    if (msg==rtEvent)
        handleSocketEvent();
    else
        handleReply(check_and_cast<TelnetPkt *>(msg));
}

void ExtTelnetClient::handleSocketEvent()
{
    // get data from buffer
    std::string text = std::string(recvBuffer, numRecvBytes);
    numRecvBytes = 0;

    // assemble and send Telnet packet
    TelnetPkt *telnetPkt = new TelnetPkt();
    telnetPkt->setPayload(text.c_str());
    telnetPkt->setName(text.c_str());
    telnetPkt->setDestAddress(srvAddr);
    telnetPkt->setSrcAddress(addr);

    send(telnetPkt,"out");
}

void ExtTelnetClient::handleReply(TelnetPkt *telnetReply)
{
    const char *reply = telnetReply->getPayload();
    rtScheduler->sendBytes(reply, strlen(reply));
    delete telnetReply;
}


