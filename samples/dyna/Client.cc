//
// This file is part of an OMNeT++/OMNEST simulation example.
//
// Copyright (C) 1992-2015 Andras Varga
//
// This file is distributed WITHOUT ANY WARRANTY. See the file
// `license' for details on this and other legal matters.
//

#include "DynaPacket_m.h"

using namespace omnetpp;

#define STACKSIZE    16384

/**
 * Client computer; see NED file for more info
 */
class Client : public cSimpleModule
{
  public:
    Client() : cSimpleModule(STACKSIZE) {}
    virtual void activity() override;
};

Define_Module(Client);

void Client::activity()
{
    // query module parameters
    simtime_t timeout = par("timeout");
    cPar& connectionIaTime = par("connIaTime");
    cPar& queryIaTime = par("queryIaTime");
    cPar& numQuery = par("numQuery");

    DynaPacket *connReq, *connAck, *discReq, *discAck;
    DynaDataPacket *query, *answer;
    int actNumQuery = 0, i = 0;
    WATCH(actNumQuery);
    WATCH(i);

    // assign address: index of Switch's gate to which we are connected
    int ownAddr = gate("port$o")->getNextGate()->getIndex();
    int serverAddr = gate("port$o")->getNextGate()->getVectorSize()-1;
    int serverprocId = 0;
    WATCH(ownAddr);
    WATCH(serverAddr);
    WATCH(serverprocId);

    for ( ; ; ) {
        if (hasGUI())
            getDisplayString().setTagArg("i", 1, "");

        // keep an interval between subsequent connections
        wait((double)connectionIaTime);

        if (hasGUI())
            getDisplayString().setTagArg("i", 1, "green");

        // connection setup
        EV << "sending DYNA_CONN_REQ\n";
        connReq = new DynaPacket("DYNA_CONN_REQ", DYNA_CONN_REQ);
        connReq->setSrcAddress(ownAddr);
        connReq->setDestAddress(serverAddr);
        send(connReq, "port$o");

        EV << "waiting for DYNA_CONN_ACK\n";
        connAck = (DynaPacket *)receive(timeout);
        if (connAck == nullptr)
            goto broken;
        serverprocId = connAck->getServerProcId();
        EV << "got DYNA_CONN_ACK, my server process is ID="
           << serverprocId << endl;
        delete connAck;

        if (hasGUI()) {
            getDisplayString().setTagArg("i", 1, "gold");
            bubble("Connected!");
        }

        // communication
        actNumQuery = (long)numQuery;
        for (i = 0; i < actNumQuery; i++) {
            EV << "sending DATA(query)\n";
            query = new DynaDataPacket("DATA(query)", DYNA_DATA);
            query->setSrcAddress(ownAddr);
            query->setDestAddress(serverAddr);
            query->setServerProcId(serverprocId);
            query->setPayload("query");
            send(query, "port$o");

            EV << "waiting for DATA(result)\n";
            answer = (DynaDataPacket *)receive(timeout);
            if (answer == nullptr)
                goto broken;
            EV << "got DATA(result)\n";
            delete answer;

            wait((double)queryIaTime);
        }

        if (hasGUI())
            getDisplayString().setTagArg("i", 1, "blue");

        // connection teardown
        EV << "sending DYNA_DISC_REQ\n";
        discReq = new DynaPacket("DYNA_DISC_REQ", DYNA_DISC_REQ);
        discReq->setSrcAddress(ownAddr);
        discReq->setDestAddress(serverAddr);
        discReq->setServerProcId(serverprocId);
        send(discReq, "port$o");

        EV << "waiting for DYNA_DISC_ACK\n";
        discAck = (DynaPacket *)receive(timeout);
        if (discAck == nullptr)
            goto broken;
        EV << "got DYNA_DISC_ACK\n";
        delete discAck;

        if (hasGUI())
            bubble("Disconnected!");

        continue;

        // error handling
      broken:
        EV << "Timeout, connection broken!\n";
        if (hasGUI())
            bubble("Connection broken!");
    }
}

