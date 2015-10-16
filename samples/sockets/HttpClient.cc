//
// This file is part of an OMNeT++/OMNEST simulation example.
//
// Copyright (C) 1992-2015 Andras Varga
//
// This file is distributed WITHOUT ANY WARRANTY. See the file
// `license' for details on this and other legal matters.
//

#include <omnetpp.h>
#include "HttpMsg_m.h"

using namespace omnetpp;

class HTTPClient : public cSimpleModule
{
  private:
    int addr;
    int srvAddr;

  protected:
    virtual void initialize() override;
    virtual void handleMessage(cMessage *msg) override;
    void sendHTTPRequest();
    void processHTTPReply(HTTPMsg *httpMsg);
};

Define_Module(HTTPClient);

void HTTPClient::initialize()
{
    addr = par("addr");
    srvAddr = par("srvAddr");

    cMessage *timer = new cMessage("timer");
    scheduleAt(simTime()+par("sendIaTime").doubleValue(), timer);
}

void HTTPClient::handleMessage(cMessage *msg)
{
    if (msg->isSelfMessage()) {
        sendHTTPRequest();
        scheduleAt(simTime()+par("sendIaTime").doubleValue(), msg);
    }
    else {
        processHTTPReply(check_and_cast<HTTPMsg *>(msg));
    }
}

void HTTPClient::sendHTTPRequest()
{
    const char *header = "GET / HTTP/1.0\r\n\r\n";

    // assemble and send HTTP request
    HTTPMsg *httpMsg = new HTTPMsg();
    httpMsg->setPayload(header);
    httpMsg->setDestAddress(srvAddr);
    httpMsg->setSrcAddress(addr);

    send(httpMsg, "g$o");
}

void HTTPClient::processHTTPReply(HTTPMsg *httpMsg)
{
    delete httpMsg;
}

