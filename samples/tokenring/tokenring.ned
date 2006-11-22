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
{
    parameters:
        int numStations; // Number of stations in the ring.
        double THT; // Token Holding Time, as defined in the protocol
        double dataRate; // Token Ring data rate in bps
        double cableDelay; // Delay of the cable connecting adjacent stations
    submodules:
        comp[numStations]: Computer {
            parameters:
                address = index;
                THT = THT;
                dataRate = dataRate;
                numStations = numStations;
                @display("i=device/pc4;q=sendQueue;p=80,80,ring");
        };
    connections:
        comp[numStations-1].out --> {  delay = cableDelay; } --> comp[0].in;

        for i=0..numStations-2 {
            comp[i].out --> {  delay = cableDelay; } --> comp[i+1].in;
        };
}


//
// Instance of the Token Ring
//
network token extends TokenRing
{
    parameters:
        numStations = default(5) @prompt("Number of stations in the ring:");
        THT = default(20ms) @prompt("Token Holding Time (s):");
        dataRate = 4000000; // 4 Mbit/s
        cableDelay = 1us;
}


