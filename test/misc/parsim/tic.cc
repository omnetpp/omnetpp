//
// This file is part of an OMNeT++/OMNEST simulation example.
//
// Copyright (C) 2010 Andras Varga
//
// This file is distributed WITHOUT ANY WARRANTY. See the file
// `license' for details on this and other legal matters.
//

#include <string.h>
#include <omnetpp.h>

using namespace omnetpp;

class Tic : public cSimpleModule
{
  protected:
    virtual void initialize();
    virtual void handleMessage(cMessage *msg);
};

Define_Module(Tic);

void Tic::initialize()
{
    if (par("initialSend").boolValue()) {
        cPacket *pkt = new cPacket(getFullName());
        send(pkt, par("outputGate").stringValue());
    }
}

void Tic::handleMessage(cMessage *msg)
{
    cPacket *pkt = check_and_cast<cPacket *>(msg);

    if (par("delete").boolValue()) {
        if (par("allowPointerAliasing").boolValue()) {
            delete pkt;
            pkt = new cPacket(getFullName());  // may get the same address as deleted pkt
        }
        else {
            pkt = new cPacket(getFullName());
            delete msg;
        }
    }
    send(pkt, par("outputGate").stringValue());
}

