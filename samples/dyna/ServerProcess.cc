//
// This file is part of an OMNeT++/OMNEST simulation example.
//
// Copyright (C) 1992-2015 Andras Varga
//
// This file is distributed WITHOUT ANY WARRANTY. See the file
// `license' for details on this and other legal matters.
//

#include "DynaPacket_m.h"

using namespace omnetpp;

#define STACKSIZE    16384

/**
 * Dynamically launched process in the server; see NED file for more info
 */
class ServerProcess : public cSimpleModule
{
  public:
    ServerProcess() : cSimpleModule(STACKSIZE) {}
    virtual void activity() override;
};

Define_Module(ServerProcess);

void ServerProcess::activity()
{
    // retrieve parameters
    cPar& processingTime = getParentModule()->par("processingTime");

    cGate *serverOutGate = getParentModule()->gate("port$o");

    int clientAddr = 0, ownAddr = 0;
    WATCH(clientAddr);
    WATCH(ownAddr);

    DynaPacket *pk;
    DynaDataPacket *datapk;

    // receive the CONN_REQ we were created to handle
    EV << "Started, waiting for DYNA_CONN_REQ\n";
    pk = check_and_cast<DynaPacket *>(receive());
    clientAddr = pk->getSrcAddress();
    ownAddr = pk->getDestAddress();

    // respond to CONN_REQ by CONN_ACK
    EV << "client is addr=" << clientAddr << ", sending DYNA_CONN_ACK\n";
    pk->setName("DYNA_CONN_ACK");
    pk->setKind(DYNA_CONN_ACK);
    pk->setSrcAddress(ownAddr);
    pk->setDestAddress(clientAddr);
    pk->setServerProcId(getId());
    sendDirect(pk, serverOutGate);

    // process data packets until DISC_REQ comes
    for ( ; ; ) {
        EV << "waiting for DATA(query) (or DYNA_DISC_REQ)\n";
        pk = check_and_cast<DynaPacket *>(receive());
        int type = pk->getKind();

        if (type == DYNA_DISC_REQ)
            break;

        if (type != DYNA_DATA)
            throw cRuntimeError("protocol error!");

        datapk = (DynaDataPacket *)pk;

        EV << "got DATA(query), processing...\n";
        wait((double)processingTime);

        EV << "sending DATA(result)\n";
        datapk->setName("DATA(result)");
        datapk->setKind(DYNA_DATA);
        datapk->setSrcAddress(ownAddr);
        datapk->setDestAddress(clientAddr);
        datapk->setPayload("result");
        sendDirect(datapk, serverOutGate);
    }

    // connection teardown in response to DISC_REQ
    EV << "got DYNA_DISC_REQ, sending DYNA_DISC_ACK\n";
    pk->setName("DYNA_DISC_ACK");
    pk->setKind(DYNA_DISC_ACK);
    pk->setSrcAddress(ownAddr);
    pk->setDestAddress(clientAddr);
    sendDirect(pk, serverOutGate);

    EV << "exiting\n";
    deleteModule();
}

