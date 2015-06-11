//
// This file is part of an OMNeT++/OMNEST simulation example.
//
// Copyright (C) 1992-2015 Andras Varga
//
// This file is distributed WITHOUT ANY WARRANTY. See the file
// `license' for details on this and other legal matters.
//

#include <ctype.h>
#include "TelnetServer.h"
#include "TelnetPkt_m.h"

Define_Module(TelnetServer);

simtime_t TelnetServer::startService(cMessage *msg)
{
    EV << "Starting service of " << msg->getName() << endl;
    return par("serviceTime").doubleValue();
}

void TelnetServer::endService(cMessage *msg)
{
    EV << "Completed service of " << msg->getName() << endl;

    TelnetPkt *telnetPkt = check_and_cast<TelnetPkt *>(msg);

    std::string reply = processChars(telnetPkt->getPayload());
    telnetPkt->setPayload(reply.c_str());
    telnetPkt->setName(reply.c_str());

    int clientAddr = telnetPkt->getSrcAddress();
    int srvAddr = telnetPkt->getDestAddress();
    telnetPkt->setDestAddress(clientAddr);
    telnetPkt->setSrcAddress(srvAddr);

    send(msg, "g$o");
}

std::string TelnetServer::processChars(const char *chars)
{
    std::string s = chars;
    for (unsigned int i = 0; i < s.length(); i++)
        s.at(i) = toupper(s.at(i));
    return s;
}

