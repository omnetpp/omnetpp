//
// This file is part of an OMNeT++ simulation example.
//
// Copyright (C) 1992-2003 Andras Varga
//
// This file is distributed WITHOUT ANY WARRANTY. See the file
// `license' for details on this and other legal matters.
//

#pragma warning(disable:4786)

#include <vector>
#include <omnetpp.h>
#include "packet_m.h"

class App : public cSimpleModule
{
  protected:
    int myAddress;
    double iatime;
    long pkCounter;

    std::vector<int> destAddresses;
    cMessage *generatePacket;
    cLongHistogram hopCounts;
    int pkReceived;

  public:
    Module_Class_Members(App,cSimpleModule,0);
    virtual void initialize();
    virtual void handleMessage(cMessage *msg);
};

Define_Module(App);


void App::initialize()
{
    myAddress = parentModule()->par("address");
    iatime = 1.0;
    pkCounter = 0;
    pkReceived = 0;

    WATCH(pkCounter);
    WATCH(myAddress);
    WATCH(iatime);
    WATCH(pkReceived);

    const char *destAddressesPar = par("destAddresses");
    char *s = strtok(const_cast<char *>(destAddressesPar)," ");
    while (s)
    {
        int addr = (int) atol(s);
        destAddresses.push_back(addr);
        s = strtok(NULL," ");
    }

    hopCounts.setName("hopCounts");
    hopCounts.setRangeAutoUpper(0,20,1.5);
    hopCounts.setNumCells(1000); // FIXME ez nagyin bena!!!

    generatePacket = new cMessage("nextPacket");
    scheduleAt(exponential(iatime), generatePacket);
}

void App::handleMessage(cMessage *msg)
{
    if (msg == generatePacket)
    {
        // Sending packet
        int destAddress = destAddresses[intuniform(0, destAddresses.size()-1)];

        char pkname[40];
        sprintf(pkname,"pk-%d-to-%d-#%d", myAddress, destAddress, pkCounter++);
        ev << "generating packet " << pkname << endl;

        Packet *pk = new Packet(pkname);
        pk->setSrcAddr(myAddress);
        pk->setDestAddr(destAddress);
        send(pk,"out");

        scheduleAt(simTime()+exponential(iatime), generatePacket);
        if (ev.isGUI()) parentModule()->bubble("Generating packet...");
    }
    else
    {
        // Handle incoming packet
        Packet *pk = check_and_cast<Packet *>(msg);
        ev << "received packet " << pk->name() << " after " << pk->getHopCount() << "hops" << endl;
        hopCounts.collect(pk->getHopCount());
        pkReceived++;
        delete pk;

        if (ev.isGUI())
        {
            parentModule()->displayString().setTagArg("i",0,"ball_s");
            parentModule()->displayString().setTagArg("i",1,"green");
            parentModule()->bubble("Arrived!");
        }
    }
}

