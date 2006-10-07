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


import "computer";


//
// Ring with a configurable number of Computers
//
module TokenRing
    parameters:
        numStations : numeric const, // Number of stations in the ring.
        THT : numeric const, // Token Holding Time, as defined in the protocol
        dataRate : numeric const, // Token Ring data rate in bps
        cableDelay : numeric const; // Delay of the cable connecting adjacent stations
    submodules:
        comp: Computer[numStations];
            parameters:
                address = index,
                THT = THT,
                dataRate = dataRate,
                numStations = numStations;
            display: "i=device/pc4;q=sendQueue;p=80,80,ring";
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


