//
// This file is part of an OMNeT++/OMNEST simulation example.
//
// Copyright (C) 1992-2005 Andras Varga
//
// This file is distributed WITHOUT ANY WARRANTY. See the file
// `license' for details on this and other legal matters.
//

#include <omnetpp.h>


class QClassifier : public cSimpleModule
{
  public:
    Module_Class_Members(QClassifier,cSimpleModule,0);
    virtual void handleMessage(cMessage *msg);
};

Define_Module(QClassifier);

void QClassifier::handleMessage(cMessage *msg)
{
    int k = msg->kind();
    if (k>=0 && k<gate("out")->size())
        send(msg, "out", k);
    else
        send(msg, "other");
}


