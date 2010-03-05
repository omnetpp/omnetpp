//
// This file is part of an OMNeT++/OMNEST simulation example.
//
// Copyright (C) 1992-2008 Andras Varga
//
// This file is distributed WITHOUT ANY WARRANTY. See the file
// `license' for details on this and other legal matters.
//


#include "HCSink.h"
#include "HCPacket_m.h"


// Module registration:
Define_Module(HCSink);


int hammingDistance(unsigned long a, unsigned long b)
{
     unsigned long d = a^b;
     int k=0;
     for (; d; d=d>>1)
         if (d&1)
             k++;
     return k;
}

void HCSink::initialize()
{
    endToEndDelaySignal = registerSignal("endToEndDelay");
    hopCountSignal = registerSignal("hopCount");
    hopRatioSignal = registerSignal("hopRatio");
}

void HCSink::handleMessage(cMessage *msg)
{
    HCPacket *pkt = check_and_cast<HCPacket *>(msg);

    // calculate statistics and record them
    simtime_t eed = pkt->getArrivalTime() - pkt->getTimestamp();
    int actualHops = pkt->getHops();
    int minHops = hammingDistance(pkt->getSrcAddress(), pkt->getDestAddress());

    emit(endToEndDelaySignal, eed);
    emit(hopCountSignal, actualHops);
    emit(hopRatioSignal, actualHops / (double)minHops );

#ifdef TRACE_MSG
    ev.printf("Message received: '%s'\n", pkt->getName());
    ev.printf("  - end-to-end delay=%g\n", eed);
    ev.printf("  - distance=%d, actual hops=%d\n", minHops, actualHops);
#endif

    // message no longer needed
    delete pkt;
}

