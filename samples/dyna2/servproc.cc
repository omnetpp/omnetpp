//-------------------------------------------------------------
// file: servproc.cc
//        (part of DYNA - an OMNeT++ demo simulation)
//-------------------------------------------------------------

#include "dynapk_n.h"

class ServerProcess : public cSimpleModule
{
    Module_Class_Members(ServerProcess,cSimpleModule,16384)
    virtual void activity();
};

Define_Module( ServerProcess );

void ServerProcess::activity()
{
    cPar& processing_time = parentModule()->par("processing_time");

    int client_addr=0, own_addr=0;
    WATCH(client_addr); WATCH(own_addr);

    DynaPacket *pk;
    DynaDataPacket *datapk;

    ev << "Started, waiting for DYNA_CONN_REQ\n";
    pk = (DynaPacket *) receive();
    client_addr = pk->getSrcAddress();
    own_addr = pk->getDestAddress();

    ev << "client is addr=" << client_addr << ", sending DYNA_CONN_ACK\n";
    pk->setName("DYNA_CONN_ACK");
    pk->setKind(DYNA_CONN_ACK);
    pk->setSrcAddress(own_addr);
    pk->setDestAddress(client_addr);
    pk->setServerProcId(id());
    send(pk, "out");

    for(;;)
    {
        ev << "waiting for DATA(query) (or DYNA_DISC_REQ)\n";
        pk = (DynaPacket *) receive();
        int type = pk->kind();

        if (type==DYNA_DISC_REQ)
            break;

        if (type!=DYNA_DATA)
            error("protocol error!");

        datapk = (DynaDataPacket *) pk;

        ev << "got DATA(query), processing...\n";
        wait( (double)processing_time );

        ev << "sending DATA(result)\n";
        datapk->setName("DATA(result)");
        datapk->setKind(DYNA_DATA);
        datapk->setSrcAddress(own_addr);
        datapk->setDestAddress(client_addr);
        datapk->setPayload("result");
        send(datapk, "out");
    }

    ev << "got DYNA_DISC_REQ, sending DYNA_DISC_ACK\n";
    pk->setName("DYNA_DISC_ACK");
    pk->setKind(DYNA_DISC_ACK);
    pk->setSrcAddress(own_addr);
    pk->setDestAddress(client_addr);
    send(pk, "out");

    ev << "exiting\n";
    pk->setName("DYNA_DISC_ACK" );
    deleteModule();
}

