//-------------------------------------------------------------
// file: server.cc
//        (part of DYNA - an OMNeT++ demo simulation)
//-------------------------------------------------------------

#include "dynapacket_m.h"

class Server : public cSimpleModule
{
    Module_Class_Members(Server,cSimpleModule,16384)
    virtual void activity();
};

Define_Module( Server );

void Server::activity()
{
    cModuleType *srvproc_type = findModuleType("ServerProcess" );

    for(;;)
    {
        DynaPacket *pk = (DynaPacket *) receive();
        int type = pk->kind();

        int serverproc_id;
        cModule *mod;

        switch (type)
        {
          case DYNA_CONN_REQ:
            mod = srvproc_type->createScheduleInit("serverproc",this);

            ev << "DYNA_CONN_REQ: Created process ID=" << mod->id() << endl;

            mod->gate("out")->setTo( gate("out") );

            sendDirect(pk, 0.0, mod, "in");
            break;

          default:
            serverproc_id = pk->getServerProcId();
            ev << "Redirecting msg to process ID="
               << serverproc_id << endl;
            mod = simulation.module( serverproc_id );
            if (!mod) {
                ev << " That process already exited, deleting msg\n";
                delete pk;
            } else {
                sendDirect(pk, 0.0, mod, "in");
            }
            break;
        }
    }
}

