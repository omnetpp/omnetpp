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

Define_Module( ServerProcess )

void ServerProcess::activity()
{
        cPar& processing_time = parentModule()->par("processing_time");

        int client_addr=0, own_addr=0;
        WATCH(client_addr); WATCH(own_addr);
        
        cMessage *msg;
        int type;

        ev << "Started, waiting for CONN_REQ\n";
        msg = receive();
        client_addr = msg->par("src");
        own_addr =    msg->par("dest");

        ev << "client is addr=" << client_addr << ", sending CONN_ACK\n";
        msg->setName( "CONN_ACK" );
        msg->setKind( CONN_ACK );
        msg->par("src")  = own_addr;
        msg->par("dest") = client_addr;
        msg->addPar("serverproc_id") = id();
        send( msg, "out" );

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

