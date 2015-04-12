//
// This file is part of an OMNeT++/OMNEST simulation example.
//
// Copyright (C) 1992-2015 Andras Varga
//
// This file is distributed WITHOUT ANY WARRANTY. See the file
// `license' for details on this and other legal matters.
//


//
// A computer in the ALOHAnet network.
//
simple AHost
    parameters:
        txRate: numeric const,  // transmission rate
        radioDelay: numeric const,  // propagation delay of radio link
        pkLenBits: numeric,  // packet length in bits
        iaTime: numeric,  // packet interarrival time
        slotTime: numeric const;  // zero means no slots (pure Aloha)
endsimple

//
// The central computer in the ALOHAnet network.
//
simple AServer
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
        server: AServer;
            parameters:
                txRate = txRate;
            display: "i=device/server";
        host: AHost[numHosts];
            parameters:
                txRate = txRate,
                slotTime = slotTime;
            display: "i=misc/node_vs";
    connections nocheck:
endmodule

network alohaNet : ALOHAnet
endnetwork

