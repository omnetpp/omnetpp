//-------------------------------------------------------------
// file: servproc.cc
//        (part of DYNA - an OMNeT++ demo simulation)
//-------------------------------------------------------------

#include "dynapacket_m.h"

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

    DynaPacket *pk;
    DynaDataPacket *datapk;

    // receive the CONN_REQ we were created to handle
    ev << "Started, waiting for DYNA_CONN_REQ\n";
    pk = (DynaPacket *) receive();
    client_addr = pk->getSrcAddress();
    own_addr = pk->getDestAddress();

    // set the module name to something informative
    char buf[30];
    sprintf(buf, "serverproc%d-clientaddr%d", id(), client_addr);
    setName(buf);

    // respond to CONN_REQ by CONN_ACK
    ev << "client is addr=" << client_addr << ", sending DYNA_CONN_ACK\n";
    pk->setName("DYNA_CONN_ACK");
    pk->setKind(DYNA_CONN_ACK);
    pk->setSrcAddress(own_addr);
    pk->setDestAddress(client_addr);
    pk->setServerProcId(id());
    send(pk, "out");

    // process data packets until DISC_REQ comes
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

    // connection teardown in response to DISC_REQ
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

