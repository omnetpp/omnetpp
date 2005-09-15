//
// This file is part of an OMNeT++/OMNEST simulation example.
//
// Copyright (C) 1992-2005 Andras Varga
//
// This file is distributed WITHOUT ANY WARRANTY. See the file
// `license' for details on this and other legal matters.
//


#include "dynapacket_m.h"

#define STACKSIZE 16384

/**
 * Client computer; see NED file for more info
 */
class Client : public cSimpleModule
{
  public:
    Client() : cSimpleModule(STACKSIZE) {}
    virtual void activity();
};

Define_Module( Client );

void Client::activity()
{
    // query module parameters
    double timeout = par("timeout");
    cPar& connectionIaTime = par("connIaTime");
    cPar& queryIaTime = par("queryIaTime");
    cPar& numQuery = par("numQuery");

    DynaPacket *connReq, *connAck, *discReq, *discAck;
    DynaDataPacket *query, *answer;
    int actNumQuery=0, i=0;
    WATCH(actNumQuery); WATCH(i);

    // assign address: index of Switch's gate to which we are connected
    int ownAddr = gate("out")->toGate()->index();
    int serverAddr = gate("out")->toGate()->size()-1;
    int serverprocId = 0;
    WATCH(ownAddr); WATCH(serverAddr); WATCH(serverprocId);

    for(;;)
    {
        if (ev.isGUI()) displayString().setTagArg("i",1,"");

        // keep an interval between subsequent connections
        wait( (double)connectionIaTime );

        if (ev.isGUI()) displayString().setTagArg("i",1,"green");

        // connection setup
        ev << "sending DYNA_CONN_REQ\n";
        connReq = new DynaPacket("DYNA_CONN_REQ", DYNA_CONN_REQ);
        connReq->setSrcAddress(ownAddr);
        connReq->setDestAddress(serverAddr);
        send( connReq, "out" );

        ev << "waiting for DYNA_CONN_ACK\n";
        connAck = (DynaPacket *) receive( timeout );
        if (connAck==NULL)
            goto broken;
        serverprocId = connAck->getServerProcId();
        ev << "got DYNA_CONN_ACK, my server process is ID="
           << serverprocId << endl;
        delete connAck;

        if (ev.isGUI())
        {
            displayString().setTagArg("i",1,"gold");
            bubble("Connected!");
        }

        // communication
        actNumQuery = (long)numQuery;
        for (i=0; i<actNumQuery; i++)
        {
            ev << "sending DATA(query)\n";
            query = new DynaDataPacket("DATA(query)", DYNA_DATA);
            query->setSrcAddress(ownAddr);
            query->setDestAddress(serverAddr);
            query->setServerProcId(serverprocId);
            query->setPayload("query");
            send(query, "out");

            ev << "waiting for DATA(result)\n";
            answer = (DynaDataPacket *) receive( timeout );
            if (answer==NULL)
                 goto broken;
            ev << "got DATA(result)\n";
            delete answer;

            wait( (double)queryIaTime );
        }

        if (ev.isGUI()) displayString().setTagArg("i",1,"blue");

        // connection teardown
        ev << "sending DYNA_DISC_REQ\n";
        discReq = new DynaPacket("DYNA_DISC_REQ", DYNA_DISC_REQ);
        discReq->setSrcAddress(ownAddr);
        discReq->setDestAddress(serverAddr);
        discReq->setServerProcId(serverprocId);
        send(discReq, "out");

        ev << "waiting for DYNA_DISC_ACK\n";
        discAck = (DynaPacket *) receive( timeout );
        if (discAck==NULL)
            goto broken;
        ev << "got DYNA_DISC_ACK\n";
        delete discAck;

        if (ev.isGUI()) bubble("Disconnected!");

        continue;

        // error handling
    broken:
        ev << "Timeout, connection broken!\n";
        if (ev.isGUI()) bubble("Connection broken!");
    }
}

