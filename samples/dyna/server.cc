//-------------------------------------------------------------
// file: server.cc
//        (part of DYNA - an OMNeT++ demo simulation)
//-------------------------------------------------------------

#include "dyna.h"

class Server : public cSimpleModule
{
        Module_Class_Members(Server,cSimpleModule,16384)
        virtual void activity();
};

Define_Module( Server )

void Server::activity()
{
        cModuleType *srvproc_type = findModuleType( "ServerProcess" );

        for(;;)
        {
             cMessage *msg = receive();
             int type = msg->kind();

             int serverproc_id;
             cModule *mod;

             switch( type )
             {
                 case CONN_REQ:
                    mod = srvproc_type->createInitStart("serverproc",this);

                    ev << "CONN_REQ: Created process ID=" << mod->id() << endl;

                    mod->gate("out")->setTo( gate("out") );

                    sendDirect( msg, 0.0, mod, "in" );
                    break;

                 default:
                    serverproc_id = msg->par("serverproc_id");
                    ev << "Redirecting msg to process ID=" 
                       << serverproc_id << endl;
                    mod = simulation.module( serverproc_id );
                    if (!mod) {
                         ev << " That process already exited, deleting msg\n";
                         delete msg;
                    } else {
                         sendDirect( msg, 0.0, mod, "in" );
                    }
                    break;
             }
        }
}

