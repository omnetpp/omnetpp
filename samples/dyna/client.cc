//-------------------------------------------------------------
// file: client.cc
//        (part of DYNA - an OMNeT++ demo simulation)
//-------------------------------------------------------------

#include "dyna.h"

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

    cMessage *conn_req, *conn_ack, *query, *answer, *disc_req, *disc_ack;
    int act_num_query=0, i=0;
    WATCH(act_num_query); WATCH(i);

    // assign address: index of Switch's gate to which we are connected
    int own_addr = gate( "out" )->toGate()->index();
    int server_addr = gate( "out" )->toGate()->size()-1;
    int serverproc_id = 0;
    WATCH(own_addr); WATCH(server_addr); WATCH(serverproc_id);

    for(;;)
    {
        // keep an interval between subsequent connections
        wait( (double)connection_ia_time );

        // connection setup
        ev << "sending CONN_REQ\n";
        conn_req = new cMessage( "CONN_REQ", CONN_REQ );
        conn_req->addPar("src") = own_addr;
        conn_req->addPar("dest") = server_addr;
        send( conn_req, "out" );

        ev << "waiting for CONN_ACK\n";
        conn_ack = receive( timeout );
        if (conn_ack==NULL)
            goto broken;
        serverproc_id = conn_ack->par("serverproc_id");
        ev << "got CONN_ACK, my server process is ID="
           << serverproc_id << endl;
        delete conn_ack;

        // communication
        act_num_query = (long)num_query;
        for (i=0; i<act_num_query; i++)
        {
            ev << "sending DATA(query)\n";
            query = new cMessage( "DATA(query)", DATA_QUERY );
            query->addPar("src")  = own_addr;
            query->addPar("dest") = server_addr;
            query->addPar("serverproc_id") = serverproc_id;
            send( query, "out" );

            ev << "waiting for DATA(result)\n";
            answer = receive( timeout );
            if (answer==NULL)
                 goto broken;
            ev << "got DATA(result)\n";
            delete answer;

            wait( (double)query_ia_time );
        }

        // connection teardown
        ev << "sending DISC_REQ\n";
        disc_req = new cMessage( "DISC_REQ", DISC_REQ );
        disc_req->addPar("src") = own_addr;
        disc_req->addPar("dest") = server_addr;
        disc_req->addPar("serverproc_id") = serverproc_id;
        send( disc_req, "out" );

        ev << "waiting for DISC_ACK\n";
        disc_ack = receive( timeout );
        if (disc_ack==NULL)
            goto broken;
        ev << "got DISC_ACK\n";
        delete disc_ack;

        continue;

        // error handling
        broken: ev << "Timeout, connection broken!\n";
    }
}

