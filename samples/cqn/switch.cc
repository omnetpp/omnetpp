//
// This file is part of an OMNeT++/OMNEST simulation example.
//
// Copyright (C) 1992-2005 Andras Varga
//
// This file is distributed WITHOUT ANY WARRANTY. See the file
// `license' for details on this and other legal matters.
//


#include <omnetpp.h>

/**
 * Sends received packets to one of the outputs; see NED file for more info
 */
class Switch : public cSimpleModule
{
  private:
    int numGates;

  protected:
    virtual void initialize();
    virtual void handleMessage(cMessage *msg);
};

Define_Module(Switch);

void Switch::initialize()
{
    numGates = gate("out")->size();
}

void Switch::handleMessage(cMessage *msg)
{
    int gate = intuniform(0,numGates-1);
    send(msg,"out",gate);
}



