//
// This file is part of an OMNeT++/OMNEST simulation example.
//
// Copyright (C) 1992-2015 Andras Varga
//
// This file is distributed WITHOUT ANY WARRANTY. See the file
// `license' for details on this and other legal matters.
//

#include <omnetpp/platdep/sockets.h>
#include <omnetpp.h>
#include "HttpMsg_m.h"
#include "SocketRTScheduler.h"

using namespace omnetpp;

/**
 * Model of a web browser.
 */
class ExtHTTPClient : public cSimpleModule
{
  private:
    cMessage *rtEvent = nullptr;
    cSocketRTScheduler *rtScheduler;

    char recvBuffer[4000];
    int numRecvBytes;

    int addr;
    int srvAddr;

  public:
    virtual ~ExtHTTPClient();

  protected:
    virtual void initialize() override;
    virtual void handleMessage(cMessage *msg) override;
    void handleSocketEvent();
    void handleReply(HTTPMsg *httpReply);
};

Define_Module(ExtHTTPClient);

ExtHTTPClient::~ExtHTTPClient()
{
    cancelAndDelete(rtEvent);
}

void ExtHTTPClient::initialize()
{
    rtEvent = new cMessage("rtEvent");
    rtScheduler = check_and_cast<cSocketRTScheduler *>(getSimulation()->getScheduler());
    rtScheduler->setInterfaceModule(this, rtEvent, recvBuffer, 4000, &numRecvBytes);

    addr = par("addr");
    srvAddr = par("srvAddr");
}

void ExtHTTPClient::handleMessage(cMessage *msg)
{
    if (msg == rtEvent)
        handleSocketEvent();
    else
        handleReply(check_and_cast<HTTPMsg *>(msg));
}

void ExtHTTPClient::handleSocketEvent()
{
    // try to find a double line feed in the input -- that's the end of the HTTP header.
    char *endHeader = nullptr;
    for (char *s = recvBuffer; s <= recvBuffer+numRecvBytes-4; s++)
        if (*s == '\r' && *(s+1) == '\n' && *(s+2) == '\r' && *(s+3) == '\n') {
            endHeader = s+4;
            break;
        }

    // we don't have a complete header yet -- keep on waiting
    if (!endHeader)
        return;
    std::string header = std::string(recvBuffer, endHeader-recvBuffer);
    // EV << header;

    // remove HTTP header from buffer
    if (endHeader == recvBuffer+numRecvBytes)
        numRecvBytes = 0;
    else {
        int bytesLeft = recvBuffer+numRecvBytes-endHeader;
        memmove(endHeader, recvBuffer, bytesLeft);
        numRecvBytes = bytesLeft;
    }

    // assemble and send HTTP request
    HTTPMsg *httpMsg = new HTTPMsg();
    httpMsg->setPayload(header.c_str());
    httpMsg->setDestAddress(srvAddr);
    httpMsg->setSrcAddress(addr);

    send(httpMsg, "g$o");
}

void ExtHTTPClient::handleReply(HTTPMsg *httpReply)
{
    const char *reply = httpReply->getPayload();
    rtScheduler->sendBytes(reply, strlen(reply));
    delete httpReply;
}

