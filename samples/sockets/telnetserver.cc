//
// This file is part of an OMNeT++/OMNEST simulation example.
//
// Copyright (C) 1992-2005 Andras Varga
//
// This file is distributed WITHOUT ANY WARRANTY. See the file
// `license' for details on this and other legal matters.
//


#include "telnetserver.h"
#include "telnetmsg_m.h"


Define_Module( TelnetServer );

simtime_t TelnetServer::startService(cMessage *msg)
{
    ev << "Starting service of " << msg->name() << endl;
    return par("serviceTime");
}

void TelnetServer::endService(cMessage *msg)
{
    ev << "Completed service of " << msg->name() << endl;

    TelnetMsg *telnetMsg = check_and_cast<TelnetMsg *>(msg);

    std::string reply = processChars(telnetMsg->getPayload());
    telnetMsg->setPayload(reply.c_str());

    int clientAddr = telnetMsg->getSrcAddress();
    int srvAddr = telnetMsg->getDestAddress();
    telnetMsg->setDestAddress(clientAddr);
    telnetMsg->setSrcAddress(srvAddr);

    send(msg, "out");
}

std::string TelnetServer::processChars(const char *chars)
{
}

