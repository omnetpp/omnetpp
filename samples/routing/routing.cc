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

#include <map>
#include <omnetpp.h>
#include "packet_m.h"


/**
 * Demonstrates static routing, utilizing the cTopology class.
 */
class Routing : public cSimpleModule
{
  private:
    int myAddress;

    typedef std::map<int,int> RoutingTable; // destaddr -> port
    RoutingTable rtable;

  protected:
    virtual void initialize();
    virtual void handleMessage(cMessage *msg);
};

Define_Module(Routing);


void Routing::initialize()
{
    myAddress = parentModule()->par("address");

    //
    // Brute force approach -- every node does topology discovery on its own,
    // and finds routes to all other nodes independently, at the beginning
    // of the simulation. This could be improved: (1) central routing database,
    // (2) on-demand route calculation
    //
    cTopology *topo = new cTopology("topo");

    topo->extractByModuleType(parentModule()->className(), NULL);
    ev << "cTopology found " << topo->nodes() << " nodes\n";

    cTopology::Node *thisNode = topo->nodeFor(parentModule());

    // find and store next hops
    for (int i=0; i<topo->nodes(); i++)
    {
        if (topo->node(i)==thisNode) continue; // skip ourselves
        topo->unweightedSingleShortestPathsTo(topo->node(i));

        if (thisNode->paths()==0) continue; // not connected

        cGate *parentModuleGate = thisNode->path(0)->localGate();
        int gateId = parentModuleGate->fromGate()->id();
        int address = topo->node(i)->module()->par("address");
        rtable[address] = gateId;
        ev << "  towards address " << address << " gateId is " << gateId << endl;
    }
    delete topo;
}

void Routing::handleMessage(cMessage *msg)
{
    Packet *pk = check_and_cast<Packet *>(msg);
    int destAddr = pk->getDestAddr();

    if (destAddr == myAddress)
    {
        ev << "local delivery of packet " << pk->name() << endl;
        send(pk,"localOut");
        return;
    }

    RoutingTable::iterator it = rtable.find(destAddr);
    if (it==rtable.end())
    {
        ev << "address " << destAddr << " unreachable, discarding packet " << pk->name() << endl;
        delete pk;
        return;
    }

    int outGate = (*it).second;
    ev << "forwarding packet " << pk->name() << " on gate id=" << outGate << endl;
    pk->setHopCount(pk->getHopCount()+1);

    if (ev.isGUI())
    {
        gate(outGate)->toGate()->displayString().setTagArg("o",0,"white");
        gate(outGate)->toGate()->displayString().setTagArg("o",1,"3");
    }

    send(pk, outGate);
}

