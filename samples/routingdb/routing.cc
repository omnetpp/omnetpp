//
// This file is part of an OMNeT++ simulation example.
//
// Copyright (C) 1992-2003 Andras Varga
//
// This file is distributed WITHOUT ANY WARRANTY. See the file
// `license' for details on this and other legal matters.
//

#pragma warning(disable:4786)

#include <map>
#include <omnetpp.h>
#include "packet_m.h"


/**
 * Central database of routing information. The model should contain
 * exactly one RoutingDB module.
 */
class RoutingDB : public cSimpleModule
{
  public:
    struct RTEntry {
        int atAddress;
        int destAddress;

        bool operator<(const RTEntry& b) const {
           return atAddress<b.atAddress || (atAddress==b.atAddress && destAddress<b.destAddress);
        }
    };
    typedef std::map<RTEntry,int> RoutingTable; // (addr+destaddr) -> port

  protected:
    RoutingTable rtable;

  public:
    Module_Class_Members(RoutingDB,cSimpleModule,0);
    virtual void initialize();
    virtual void handleMessage(cMessage *msg);

    /**
     * Returns the next hop for routing a packet at given node towards the
     * given destination address. Return value: the gate id.
     */
    virtual int getNextHop(int atAddress, int destAddress);
};


Define_Module(RoutingDB);


void RoutingDB::initialize()
{
    cTopology *topo = new cTopology("topo");

    // this can probably be made more flexible
    topo->extractByModuleType(par("nodeType"), NULL);
    ev << "cTopology found " << topo->nodes() << " nodes\n";

    // find and store next hops
    //
    // note that we do route calculation (call unweightedSingleShortestPathsTo())
    // only n times, while per-node calculation would require n^2 invocations.
    //
    for (int i=0; i<topo->nodes(); i++)
    {
        RTEntry key;
        cTopology::Node *destNode = topo->node(i);
        key.destAddress = destNode->module()->par("address");

        topo->unweightedSingleShortestPathsTo(destNode);

        for (int j=0; j<topo->nodes(); j++)
        {
            if (i==j) continue;
            cTopology::Node *atNode = topo->node(j);
            if (atNode->paths()==0) continue; // not connected

            key.atAddress = atNode->module()->par("address");
            int gateId = atNode->path(0)->localGate()->id();
            rtable[key] = gateId;
            ev << "  from " << key.atAddress << " towards " << key.destAddress << " gateId is " << gateId << endl;
        }
    }
    delete topo;
}

int RoutingDB::getNextHop(int atAddress, int destAddress)
{
    Enter_Method("getNextHop(at=%d,dest=%d)",atAddress,destAddress);

    RTEntry key;
    key.atAddress = atAddress;
    key.destAddress = destAddress;

    RoutingTable::iterator it = rtable.find(key);
    if (it==rtable.end())
        return -1;

    int outGate = (*it).second;
    return outGate;
}

void RoutingDB::handleMessage(cMessage *msg)
{
    error("cannot handle messages, must talk to it via method calls");
}

//----

/**
 * Performs next-hop routing of packets. Relies on the routing database
 * (RoutingDB) for doing that.
 */
class Routing : public cSimpleModule
{
  protected:
    int myAddress;
    RoutingDB *routingDB;

  public:
    Module_Class_Members(Routing,cSimpleModule,0);
    virtual void initialize();
    virtual void handleMessage(cMessage *msg);
};

Define_Module(Routing);


void Routing::initialize()
{
    myAddress = parentModule()->par("address");
    const char *routingDBPath = par("routingDBPath");
    routingDB = check_and_cast<RoutingDB *>(simulation.moduleByPath(routingDBPath));
}

void Routing::handleMessage(cMessage *msg)
{
    Packet *pk = check_and_cast<Packet *>(msg);
    int destAddr = pk->getDestAddr();

    if (destAddr==myAddress)
    {
        ev << "local delivery of packet " << pk->name() << endl;
        send(pk,"localOut");
        return;
    }

    int parentGateId = routingDB->getNextHop(myAddress, destAddr);
    if (parentGateId==-1)
    {
        ev << "address " << destAddr << " unreachable, discarding packet " << pk->name() << endl;
        delete pk;
        return;
    }

    cGate *outGate = parentModule()->gate(parentGateId)->fromGate();
    ev << "forwarding packet " << pk->name() << " on parent's gate id=" << parentGateId << endl;
    pk->setHopCount(pk->getHopCount()+1);

    if (ev.isGUI())
    {
        outGate->toGate()->displayString().setTagArg("o",0,"white");
        outGate->toGate()->displayString().setTagArg("o",1,"3");
    }

    send(pk, outGate);
}

