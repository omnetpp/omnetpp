//-------------------------------------------------------------
// file: servproc.cc
//        (part of DYNA - an OMNeT++ demo simulation)
//-------------------------------------------------------------

#include "dyna.h"

class ServerProcess : public cSimpleModule
{
    Module_Class_Members(ServerProcess,cSimpleModule,16384)
    virtual void activity();
};

Define_Module( ServerProcess );

void ServerProcess::activity()
{
    // retrieve parameters
    cPar& processing_time = parentModule()->par("processing_time");

    int client_addr=0, own_addr=0;
    WATCH(client_addr); WATCH(own_addr);

    cMessage *msg;

    // receive the CONN_REQ we were created to handle
    ev << "Started, waiting for CONN_REQ\n";
    msg = receive();
    client_addr = msg->par("src");
    own_addr =    msg->par("dest");

    // set the module name to something informative
    char buf[30];
    sprintf(buf, "serverproc%d-clientaddr%d", id(), client_addr);
    setName(buf);

    // respond to CONN_REQ by CONN_ACK
    ev << "client is addr=" << client_addr << ", sending CONN_ACK\n";
    msg->setName( "CONN_ACK" );
    msg->setKind( CONN_ACK );
    msg->par("src")  = own_addr;
    msg->par("dest") = client_addr;
    msg->addPar("serverproc_id") = id();
    send( msg, "out" );

    // process data packets until DISC_REQ comes
    for(;;)
    {
        ev << "waiting for DATA(query) (or DISC_REQ)\n";
        msg = receive();
        int type = msg->kind();

        if (type==DISC_REQ)
            break;

        if (type!=DATA_QUERY)
            error( "protocol error!" );

        ev << "got DATA(query), processing...\n";
        wait( (double)processing_time );

        ev << "sending DATA(result)\n";
        msg->setName( "DATA(result)" );
        msg->setKind( DATA_RESULT );
        msg->par("src")  = own_addr;
        msg->par("dest") = client_addr;
        send( msg, "out" );
    }

    // connection teardown in response to DISC_REQ
    ev << "got DISC_REQ, sending DISC_ACK\n";
    msg->setName( "DISC_ACK" );
    msg->setKind( DISC_ACK );
    msg->par("src")  = own_addr;
    msg->par("dest") = client_addr;
    send( msg, "out" );

    ev << "exiting\n";
    msg->setName( "DISC_ACK" );
    deleteModule();
}

