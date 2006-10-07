//
// This file is part of an OMNeT++/OMNEST simulation example.
//
// Copyright (C) 1992-2005 Andras Varga
//
// This file is distributed WITHOUT ANY WARRANTY. See the file
// `license' for details on this and other legal matters.
//


#include <stdio.h>
#include <omnetpp.h>
#include "gensink.h"
#include "hcpacket_m.h"

// Turn on code that prints debug messages
#define TRACE_MSG

// Module registration:
Define_Module( HCSink );
Define_Module( HCGenerator );

//
// Activities of the simple modules
//
void HCGenerator::activity()
{
    int num_stations = par("num_stations");
    int my_address = par("address");
    cPar& ia_time = par("ia_time"); // take by ref since it can be random

    for (int i=0;;i++)
    {
        // select destination randomly (but not the local station)
        int dest = intrand(num_stations-1);
        if (dest>=my_address) dest++;

        // create packet
        char pktname[30];
        sprintf(pktname, "%d-->%d", my_address,dest);
        HCPacket *pkt = new HCPacket(pktname);
        pkt->setSrcAddress(my_address);
        pkt->setDestAddress(dest);
        pkt->setHops(0L);
        pkt->setTimestamp();

        // send out the message
#ifdef TRACE_MSG
        ev.printf("gen[%d]: Generated new pkt: '%s'\n",my_address, pkt->name());
#endif
        send(pkt, "out");

        // wait between messages
        //
        // Note that ia_time is a reference to the module parameter "ia_time"
        // that will be evaluated here. The module parameter can also take
        // a random value (for example: truncnormal(0.5,0.1) ).
        wait( ia_time );
    }
}

int hammingDistance(unsigned long a, unsigned long b)
{
     unsigned long d = a^b;
     int k=0;
     for (; d; d=d>>1)
         if (d&1)
             k++;
     return k;
}

void HCSink::activity()
{
    int my_address = par("address");

    cOutVector eed_vec("end-to-end delay",1);
    cOutVector hops_vec("hops",1);
    cOutVector hopratio_vec("actual/min hops ratio",1);

    for(;;)
    {
        // receive a message and cast it to HCPacket
        cMessage *msg = receive();
        HCPacket *pkt = check_and_cast<HCPacket *>(msg);

        // calculate statistics and record in output vector file
        simtime_t eed = pkt->arrivalTime() - pkt->timestamp();
        int acthops = pkt->getHops();
        int minhops = hammingDistance(pkt->getSrcAddress(), pkt->getDestAddress());

        eed_vec.record( eed );
        hops_vec.record( acthops );
        hopratio_vec.record( acthops/(double)minhops );

#ifdef TRACE_MSG
        ev.printf("sink[%d]: Message received: '%s'\n", my_address, pkt->name());
        ev.printf("sink[%d]:   - end-to-end delay=%g\n", my_address, eed);
        ev.printf("sink[%d]:   - distance=%d, actual hops=%d\n", my_address, minhops, acthops);
#endif

        // message no longer needed
        delete pkt;
    }
}

