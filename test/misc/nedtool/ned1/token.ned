//
// This file is part of an OMNeT++/OMNEST simulation example.
//
// Copyright (C) 1992-2015 Andras Varga
//
// This file is distributed WITHOUT ANY WARRANTY. See the file
// `license' for details on this and other legal matters.
//
// Authors: Gabor Lencse and Andras Varga (TU Budapest),
// based on the code from Maurits Andre, George van Montfort,
// and Gerard van de Weerd (TU Delft).
//


//
// Implements the Token Ring protocol.
//
simple TokenRingMAC
    parameters:
        // Token Ring data rate in bps. Standard values are 4 or 16 Mbps.
        dataRate: numeric const,
        // Token Holding Time, as defined in the protocol.
        THT: numeric const,
        // MAC address of this station, modeled with an integer.
        address: numeric const,
        // Max queue size in packets.
        queueMaxLen: numeric const;
    gates:
        in: phy_in;
        in: from_hl;
        out: phy_out;
        out: to_hl;
endsimple


//
// Generates data packets for MAC.
//
simple Generator
    parameters:
        // Number of stations. Destination MAC address will be a random
        // [0,numStatons-1] integer, omitting our own address.
        numStations: numeric const,
        // MAC Address of this station.
        address: numeric const,
        // Number of messages to generate.
        numMessages: numeric const,
        // Message length in bytes. It can be a random variate.
        messageLength: numeric,
        // Time between two messages. It can be a random variate.
        interArrivalTime: numeric;
    gates:
        out: out;
endsimple


//
// Destroys the packets and creates statistics.
//
simple Sink
    gates:
        in: in;
endsimple


//
// A node in a Token Ring LAN. Each instance will be connected to its right neighbour.
//
module Computer
    parameters:
        address: numeric const, // MAC Address of this station.
        dataRate: numeric const, // Token Ring data rate in bps.
        THT: numeric const, // Token Holding Time, as defined in the protocol.
        numStations: numeric const; // Number of stations. MAC addresses are
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
            display: "p=137,166;i=block/activeq;q=sendQueue";
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


//
// Ring with a configurable number of Computers
//
module TokenRing
    parameters:
        numStations: numeric const, // Number of stations in the ring.
        THT: numeric const, // Token Holding Time, as defined in the protocol
        dataRate: numeric const, // Token Ring data rate in bps
        cableDelay: numeric const; // Delay of the cable connecting adjacent stations
    submodules:
        comp: Computer[numStations];
            parameters:
                address = index,
                THT = THT,
                dataRate = dataRate,
                numStations = numStations;
            display: "i=device/pc4;q=sendQueue;p=183,88,ring";
    connections:
        comp[numStations - 1].out --> delay cableDelay --> comp[0].in;

        for i=0..numStations - 2 do
            comp[i].out --> delay cableDelay --> comp[i + 1].in;
        endfor;
endmodule


//
// Instance of the Token Ring
//
network token : TokenRing
    parameters:
        numStations = input(5, "Number of stations in the ring:"),
        THT = input(20ms,"Token Holding Time (s):"),
        dataRate = 4000000, // 4 Mbit/s
        cableDelay = 1us;
endnetwork


