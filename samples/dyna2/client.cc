//-------------------------------------------------------------
// file: client.cc
//        (part of DYNA - an OMNeT++ demo simulation)
//-------------------------------------------------------------

#include "dynapacket_m.h"

class Client : public cSimpleModule
{
    Module_Class_Members(Client,cSimpleModule,16384)
    virtual void activity();
};

Define_Module( Client );

void Client::activity()
{
    // query module parameters
    double timeout = par( "timeout" );
    cPar& connection_ia_time = par( "conn_ia_time" );
    cPar& query_ia_time = par( "query_ia_time" );
    cPar& num_query = par( "num_query" );

    DynaPacket *conn_req, *conn_ack, *disc_req, *disc_ack;
    DynaDataPacket *query, *answer;
    int act_num_query=0, i=0;
    WATCH(act_num_query); WATCH(i);

    // assign address: index of Switch's gate to which we are connected
    int own_addr = gate("out")->toGate()->index();
    int server_addr = gate("out")->toGate()->size()-1;
    int serverproc_id = 0;
    WATCH(own_addr); WATCH(server_addr); WATCH(serverproc_id);

    for(;;)
    {
        // keep an interval between subsequent connections
        wait( (double)connection_ia_time );

        // connection setup
        ev << "sending DYNA_CONN_REQ\n";
        conn_req = new DynaPacket("DYNA_CONN_REQ");
        conn_req->setKind(DYNA_CONN_REQ);
        conn_req->setSrcAddress(own_addr);
        conn_req->setDestAddress(server_addr);
        send( conn_req, "out" );

        ev << "waiting for DYNA_CONN_ACK\n";
        conn_ack = (DynaPacket *) receive( timeout );
        if (conn_ack==NULL)
            goto broken;
        serverproc_id = conn_ack->getServerProcId();
        ev << "got DYNA_CONN_ACK, my server process is ID="
           << serverproc_id << endl;
        delete conn_ack;

        // communication
        act_num_query = (long)num_query;
        for (i=0; i<act_num_query; i++)
        {
            ev << "sending DATA(query)\n";
            query = new DynaDataPacket("DATA(query)");
            query->setKind(DYNA_DATA);
            query->setSrcAddress(own_addr);
            query->setDestAddress(server_addr);
            query->setServerProcId(serverproc_id);
            query->setPayload("query");
            send(query, "out");

            ev << "waiting for DATA(result)\n";
            answer = (DynaDataPacket *) receive( timeout );
            if (answer==NULL)
                 goto broken;
            ev << "got DATA(result)\n";
            delete answer;

            wait( (double)query_ia_time );
        }

        // connection teardown
        ev << "sending DYNA_DISC_REQ\n";
        disc_req = new DynaPacket("DYNA_DISC_REQ");
		disc_req->setKind(DYNA_DISC_REQ);
        disc_req->setSrcAddress(own_addr);
        disc_req->setDestAddress(server_addr);
        disc_req->setServerProcId(serverproc_id);
        send(disc_req, "out");

        ev << "waiting for DYNA_DISC_ACK\n";
        disc_ack = (DynaPacket *) receive( timeout );
        if (disc_ack==NULL)
            goto broken;
        ev << "got DYNA_DISC_ACK\n";
        delete disc_ack;

        continue;

        // error handling
        broken: ev << "Timeout, connection broken!\n";
    }
}

