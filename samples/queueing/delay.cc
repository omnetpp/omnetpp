//
// This file is part of an OMNeT++ simulation example.
//
// Copyright (C) 1992-2004 Andras Varga
//
// This file is distributed WITHOUT ANY WARRANTY. See the file
// `license' for details on this and other legal matters.
//

#include <omnetpp.h>


class QDelay : public cSimpleModule
{
  protected:
    int currentlyStored;
  public:
    Module_Class_Members(QDelay,cSimpleModule,0);
    virtual void initialize();
    virtual void handleMessage(cMessage *msg);
};

Define_Module(QDelay);

void QDelay::initialize()
{
    currentlyStored = 0;
    WATCH(currentlyStored);
}

void QDelay::handleMessage(cMessage *msg)
{
    if (!msg->isSelfMessage())
    {
        currentlyStored++;
        scheduleAt(simTime()+(double)par("delayBy"), msg);
    }
    else
    {
        currentlyStored--;
        send(msg, "out");
    }

    if (ev.isGUI()) displayString().setTagArg("i",1, currentlyStored==0 ? "" : "cyan4");
}


