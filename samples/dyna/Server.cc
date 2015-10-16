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

/**
 * The server computer; see NED file for more info
 */
class Server : public cSimpleModule
{
  private:
    cModuleType *srvProcType;

  protected:
    virtual void initialize() override;
    virtual void handleMessage(cMessage *msg) override;
};

Define_Module(Server);

void Server::initialize()
{
    srvProcType = cModuleType::find("ServerProcess");
}

void Server::handleMessage(cMessage *msg)
{
    DynaPacket *pk = check_and_cast<DynaPacket *>(msg);

    if (pk->getKind() == DYNA_CONN_REQ) {
        cModule *mod = srvProcType->createScheduleInit("serverproc", this);
        EV << "DYNA_CONN_REQ: Created process ID=" << mod->getId() << endl;
        sendDirect(pk, mod, "in");
    }
    else {
        int serverProcId = pk->getServerProcId();
        EV << "Redirecting msg to process ID=" << serverProcId << endl;
        cModule *mod = getSimulation()->getModule(serverProcId);
        if (!mod) {
            EV << " That process already exited, deleting msg\n";
            delete pk;
        }
        else {
            sendDirect(pk, mod, "in");
        }
    }
}

