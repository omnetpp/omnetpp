//
// This file is part of an OMNeT++ simulation example.
//
// Copyright (C) 1992-2004 Andras Varga
//
// This file is distributed WITHOUT ANY WARRANTY. See the file
// `license' for details on this and other legal matters.
//

#include <omnetpp.h>


class QSource : public cSimpleModule
{
  public:
    Module_Class_Members(QSource,cSimpleModule,0);

    cMessage *sendMessage;

    // the virtual functions
    virtual void initialize();
    virtual void handleMessage(cMessage *msg);
};

Define_Module(QSource);

void QSource::initialize()
{
    sendMessage = new cMessage("sendMessage");
    scheduleAt(0.0, sendMessage);
}

void QSource::handleMessage(cMessage *msg)
{
    ASSERT(msg==sendMessage);

    cMessage *m = new cMessage("job");
    m->setKind(intrand(4));  // FIXME use msgKindProbabilities

    send(m, "out");

    scheduleAt(simTime()+(double)par("sendIATime"), sendMessage);
}


