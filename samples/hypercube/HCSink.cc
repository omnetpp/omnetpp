//
// This file is part of an OMNeT++/OMNEST simulation example.
//
// Copyright (C) 1992-2005 Andras Varga
//
// This file is distributed WITHOUT ANY WARRANTY. See the file
// `license' for details on this and other legal matters.
//


#include "HCSink.h"
#include "HCPacket_m.h"


// Module registration:
Define_Module( HCSink );


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

    cOutVector eed_vec("end-to-end delay");
    cOutVector hops_vec("hops");
    cOutVector hopratio_vec("actual/min hops ratio");

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

