//
// This file is part of an OMNeT++/OMNEST simulation example.
//
// Copyright (C) 1992-2015 Andras Varga
//
// This file is distributed WITHOUT ANY WARRANTY. See the file
// `license' for details on this and other legal matters.
//

#include "HCGenerator.h"
#include "HCPacket_m.h"

Define_Module(HCGenerator);

void HCGenerator::activity()
{
    int numStations = par("numStations");
    int my_address = par("address");
    cPar& iaTime = par("iaTime");  // take by ref since it can be random

    for (int i = 0; ; i++) {
        // select destination randomly (but not the local station)
        int dest = intrand(numStations-1);
        if (dest >= my_address)
            dest++;

        // create packet
        char pktname[30];
        sprintf(pktname, "%d-->%d", my_address, dest);
        HCPacket *pkt = new HCPacket(pktname);
        pkt->setSrcAddress(my_address);
        pkt->setDestAddress(dest);
        pkt->setHops(0L);
        pkt->setTimestamp();
        EV << "Generated " << pkt->getName() << endl;

        // send out the message
        send(pkt, "out");

        // wait between messages
        //
        // Note that iaTime is a reference to the module parameter "iaTime"
        // that will be evaluated here. The module parameter can also take
        // a random value (for example: truncnormal(0.5,0.1) ).
        wait(iaTime.doubleValue());
    }
}

