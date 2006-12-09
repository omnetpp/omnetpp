//
// This file is part of an OMNeT++/OMNEST simulation example.
//
// Copyright (C) 1992-2005 Andras Varga
//
// This file is distributed WITHOUT ANY WARRANTY. See the file
// `license' for details on this and other legal matters.
//


//
// A computer in the ALOHAnet network.
//
simple AHost
{
    parameters:
        double txRate @unit(bps);  // transmission rate
        double radioDelay;  // propagation delay of radio link
        volatile int pkLenBits;  // packet length in bits
        volatile double iaTime;  // packet interarrival time
        double slotTime;  // zero means no slots (pure Aloha)
}

//
// The central computer in the ALOHAnet network.
//
simple AServer
{
    parameters:
        double txRate @unit(bps); // transmission rate (needed to calculate lengths of packet receptions)
    gates:
        input in;
}

//
// ALOHAnet consists of hosts which talk to the central "server" via
// Aloha or Slotted Aloha protocol
//
module ALOHAnet
{
    parameters:
        int numHosts;  // number of hosts
        double txRate @unit(bps);  // transmission rate
        double slotTime;  // zero means no slots (pure Aloha)
        @display("o=,white");
    submodules:
        server: AServer {
            parameters:
                txRate = txRate;
                @display("i=device/antennatower_l");
        };
        host[numHosts]: AHost {
            parameters:
                txRate = txRate;
                slotTime = slotTime;
                @display("i=device/pc_s");
        };
    connections allowunconnected:
}

network alohaNet extends ALOHAnet
{
}

