//
// This file is part of an OMNeT++/OMNEST simulation example.
//
// Copyright (C) 1992-2005 Andras Varga
//
// This file is distributed WITHOUT ANY WARRANTY. See the file
// `license' for details on this and other legal matters.
//

#include <omnetpp.h>


class QSource : public cSimpleModule
{
  private:
    cMessage *sendMessage;

  public:
    QSource();
    virtual ~QSource();

  protected:
    virtual void initialize();
    virtual void handleMessage(cMessage *msg);
};

Define_Module(QSource);

QSource::QSource()
{
    sendMessage = NULL;
}

QSource::~QSource()
{
    cancelAndDelete(sendMessage);
}

void QSource::initialize()
{
    sendMessage = new cMessage("sendMessage");
    scheduleAt(0.0, sendMessage);
}

void QSource::handleMessage(cMessage *msg)
{
    ASSERT(msg==sendMessage);

    cMessage *m = new cMessage("job");
    m->setKind(intrand(4));

    send(m, "out");

    scheduleAt(simTime()+(double)par("sendIATime"), sendMessage);
}


