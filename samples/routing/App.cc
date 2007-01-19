//
// This file is part of an OMNeT++/OMNEST simulation example.
//
// Copyright (C) 1992-2005 Andras Varga
//
// This file is distributed WITHOUT ANY WARRANTY. See the file
// `license' for details on this and other legal matters.
//

#ifdef _MSC_VER
#pragma warning(disable:4786)
#endif

#include <vector>
#include <omnetpp.h>
#include "packet_m.h"


/**
 * Generates traffic for the network.
 */
class App : public cSimpleModule
{
  private:
    // configuration:
    int myAddress;
    double iatime;
    long pkCounter;

    std::vector<int> destAddresses;
    cMessage *generatePacket;

    // statistics:
    cLongHistogram hopCounts;
    int pkReceived;

  public:
    App();
    virtual ~App();

  protected:
    virtual void initialize();
    virtual void handleMessage(cMessage *msg);
};

Define_Module(App);


App::App()
{
    generatePacket = NULL;
}

App::~App()
{
    cancelAndDelete(generatePacket);
}

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
    cStringTokenizer tokenizer(destAddressesPar);
    const char *token;
    while ((token = tokenizer.nextToken())!=NULL)
        destAddresses.push_back(atoi(token));

    hopCounts.setName("hopCounts");
    hopCounts.setRangeAutoUpper(0,20,1.5);
    hopCounts.setNumCells(1000);

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
        sprintf(pkname,"pk-%d-to-%d-#%ld", myAddress, destAddress, pkCounter++);
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
            parentModule()->displayString().setTagArg("i",1,"green");
            parentModule()->bubble("Arrived!");
        }
    }
}

