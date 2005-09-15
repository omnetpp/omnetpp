//
// This file is part of an OMNeT++/OMNEST simulation example.
//
// Copyright (C) 1992-2005 Andras Varga
//
// This file is distributed WITHOUT ANY WARRANTY. See the file
// `license' for details on this and other legal matters.
//
// Authors: Gabor Lencse and Andras Varga (TU Budapest),
// based on the code from Maurits Andre, George van Montfort,
// and Gerard van de Weerd (TU Delft).
//


import "gen";
import "sink";
import "token";


//
// A node in a Token Ring LAN. Each instance will be connected to its right neighbour.
//
module Computer
    parameters:
        address : numeric const, // MAC Address of this station.
        dataRate : numeric const, // Token Ring data rate in bps.
        THT : numeric const, // Token Holding Time, as defined in the protocol.
        numStations : numeric const; // Number of stations. MAC addresses are
                                     // [0,numStatons-1] integers.
    gates:
        in: in;
        out: out;
    submodules:
        mac: TokenRingMAC;
            parameters:
                address = address,
                dataRate = dataRate,
                THT = THT;
            display: "p=137,166;i=block/queue;q=sendQueue";
        gen: Generator;
            parameters:
                numStations = numStations,
                address = address;
            display: "p=79,60;i=block/source";
        sink: Sink;
            display: "p=196,60;i=block/sink";
    connections:
        in --> mac.phy_in;
        mac.phy_out --> out;
        mac.to_hl --> sink.in;
        gen.out --> mac.from_hl;
endmodule
