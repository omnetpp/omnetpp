//
// This file is part of an OMNeT++/OMNEST simulation example.
//
// Copyright (C) 1992-2005 Andras Varga
//
// This file is distributed WITHOUT ANY WARRANTY. See the file
// `license' for details on this and other legal matters.
//


#include <omnetpp.h>


class HTTPClient : public cSimpleModule
{
  public:
    Module_Class_Members(HTTPClient,cSimpleModule,0);

    virtual void initialize();
    virtual void handleMessage(cMessage *msg);
};

Define_Module(HTTPClient);

void HTTPClient::initialize()
{
}

void HTTPClient::handleMessage(cMessage *msg)
{
}

