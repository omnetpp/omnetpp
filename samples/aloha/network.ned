//
// This file is part of an OMNeT++ simulation example.
//
// Copyright (C) 1992-2003 Andras Varga
//
// This file is distributed WITHOUT ANY WARRANTY. See the file
// `license' for details on this and other legal matters.
//


//
// Host
//
simple Host
    parameters:
        txRate: numeric const,  // transmission rate
        radioDelay: numeric const,  // propagation delay of radio link
        pkLenBits: numeric,  // packet length in bits
        iaTime: numeric,  // packet interarrival time
        slotTime: numeric const;  // zero means no slots (pure Aloha)
endsimple

//
// This is the central computer in the ALOHAnet network.
//
simple Server
    parameters:
        txRate: numeric const; // transmission rate (needed to calculate lengths of packet receptions)
    gates:
        in: in;
endsimple

//
// ALOHAnet consists of hosts which talk to the central "server" via
// Aloha or Slotted Aloha protocol
//
module ALOHAnet
    parameters:
        numHosts: numeric,  // number of hosts
        txRate: numeric,  // transmission rate
        slotTime: numeric const;  // zero means no slots (pure Aloha)
    submodules:
        server: Server;
            parameters:
                txRate = txRate;
            display: "i=server1";
        host: Host[numHosts];
            parameters:
                txRate = txRate,
                slotTime = slotTime;
            display: "i=ball_s";
endmodule

network alohaNet : ALOHAnet
endnetwork

