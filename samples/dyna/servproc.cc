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
 * Dynamically launched process in the server; see NED file for more info
 */
class ServerProcess : public cSimpleModule
{
  public:
    ServerProcess() : cSimpleModule(STACKSIZE) {}
    virtual void activity();
};

Define_Module( ServerProcess );

void ServerProcess::activity()
{
    // retrieve parameters
    cPar& processingTime = parentModule()->par("processingTime");

    cGate *serverOutGate = parentModule()->gate("out");

    int clientAddr=0, ownAddr=0;
    WATCH(clientAddr); WATCH(ownAddr);

    DynaPacket *pk;
    DynaDataPacket *datapk;

    // receive the CONN_REQ we were created to handle
    ev << "Started, waiting for DYNA_CONN_REQ\n";
    pk = (DynaPacket *) receive();
    clientAddr = pk->getSrcAddress();
    ownAddr = pk->getDestAddress();

    // set the module name to something informative
    char buf[30];
    sprintf(buf, "serverproc%d-clientaddr%d", id(), clientAddr);
    setName(buf);

    // respond to CONN_REQ by CONN_ACK
    ev << "client is addr=" << clientAddr << ", sending DYNA_CONN_ACK\n";
    pk->setName("DYNA_CONN_ACK");
    pk->setKind(DYNA_CONN_ACK);
    pk->setSrcAddress(ownAddr);
    pk->setDestAddress(clientAddr);
    pk->setServerProcId(id());
    sendDirect(pk, 0, serverOutGate);

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
        wait( (double)processingTime );

        ev << "sending DATA(result)\n";
        datapk->setName("DATA(result)");
        datapk->setKind(DYNA_DATA);
        datapk->setSrcAddress(ownAddr);
        datapk->setDestAddress(clientAddr);
        datapk->setPayload("result");
        sendDirect(datapk, 0, serverOutGate);
    }

    // connection teardown in response to DISC_REQ
    ev << "got DYNA_DISC_REQ, sending DYNA_DISC_ACK\n";
    pk->setName("DYNA_DISC_ACK");
    pk->setKind(DYNA_DISC_ACK);
    pk->setSrcAddress(ownAddr);
    pk->setDestAddress(clientAddr);
    sendDirect(pk, 0, serverOutGate);

    ev << "exiting\n";
    deleteModule();
}

