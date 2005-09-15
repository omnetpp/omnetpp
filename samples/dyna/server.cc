//
// This file is part of an OMNeT++/OMNEST simulation example.
//
// Copyright (C) 1992-2005 Andras Varga
//
// This file is distributed WITHOUT ANY WARRANTY. See the file
// `license' for details on this and other legal matters.
//


#include "dynapacket_m.h"

/**
 * The server computer; see NED file for more info
 */
class Server : public cSimpleModule
{
  private:
    cModuleType *srvProcType;

  protected:
    virtual void initialize();
    virtual void handleMessage(cMessage *msg);
};

Define_Module(Server);


void Server::initialize()
{
    srvProcType = findModuleType("ServerProcess");
}

void Server::handleMessage(cMessage *msg)
{
    DynaPacket *pk = check_and_cast<DynaPacket *>(msg);

    if (pk->kind()==DYNA_CONN_REQ)
    {
        cModule *mod = srvProcType->createScheduleInit("serverproc",this);
        ev << "DYNA_CONN_REQ: Created process ID=" << mod->id() << endl;
        sendDirect(pk, 0.0, mod, "in");
    }
    else
    {
        int serverProcId = pk->getServerProcId();
        ev << "Redirecting msg to process ID=" << serverProcId << endl;
        cModule *mod = simulation.module(serverProcId);
        if (!mod) {
            ev << " That process already exited, deleting msg\n";
            delete pk;
        } else {
            sendDirect(pk, 0.0, mod, "in");
        }
    }
}

