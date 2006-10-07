//
// This file is part of an OMNeT++/OMNEST simulation example.
//
// Copyright (C) 1992-2005 Andras Varga
//
// This file is distributed WITHOUT ANY WARRANTY. See the file
// `license' for details on this and other legal matters.
//


#include <omnetpp.h>
#include "httpmsg_m.h"
#include "csocketrtscheduler.h"


/**
 * Model of a web browser.
 */
class ExtHTTPClient : public cSimpleModule
{
  private:
    cMessage *rtEvent;
    cSocketRTScheduler *rtScheduler;

    char recvBuffer[4000];
    int numRecvBytes;

    int addr;
    int srvAddr;

  public:
    ExtHTTPClient();
    virtual ~ExtHTTPClient();

  protected:
    virtual void initialize();
    virtual void handleMessage(cMessage *msg);
    void handleSocketEvent();
    void handleReply(HTTPMsg *httpReply);
};

Define_Module(ExtHTTPClient);

ExtHTTPClient::ExtHTTPClient()
{
    rtEvent = NULL;
}

ExtHTTPClient::~ExtHTTPClient()
{
    cancelAndDelete(rtEvent);
}

void ExtHTTPClient::initialize()
{
    rtEvent = new cMessage("rtEvent");
    rtScheduler = check_and_cast<cSocketRTScheduler *>(simulation.scheduler());
    rtScheduler->setInterfaceModule(this, rtEvent, recvBuffer, 4000, &numRecvBytes);

    addr = par("addr");
    srvAddr = par("srvAddr");
}

void ExtHTTPClient::handleMessage(cMessage *msg)
{
    if (msg==rtEvent)
        handleSocketEvent();
    else
        handleReply(check_and_cast<HTTPMsg *>(msg));
}

void ExtHTTPClient::handleSocketEvent()
{
    // try to find a double line feed in the input -- that's the end of the HTTP header.
    char *endHeader = NULL;
    for (char *s=recvBuffer; s<=recvBuffer+numRecvBytes-4; s++)
        if (*s=='\r' && *(s+1)=='\n' && *(s+2)=='\r' && *(s+3)=='\n')
            {endHeader = s+4; break;}

    // we don't have a complete header yet -- keep on waiting
    if (!endHeader)
        return;
    std::string header = std::string(recvBuffer, endHeader-recvBuffer);
    //ev << header;

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

    send(httpMsg,"out");
}

void ExtHTTPClient::handleReply(HTTPMsg *httpReply)
{
    const char *reply = httpReply->getPayload();
    rtScheduler->sendBytes(reply, strlen(reply));
    delete httpReply;
}


