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


//
// Ring with a configurable number of stations
//
network TokenRing
{
    parameters:
        int numStations = default(5) @prompt("Number of stations in the ring");
        double THT = default(20ms) @prompt("Token Holding Time (s)");
        double dataRate = 4 Mbps; // Token Ring data rate
        double cableDelay = 1us; // Delay of the cable connecting adjacent stations
    submodules:
        comp[numStations]: Station {
            parameters:
                address = index;
                THT = THT;
                dataRate = dataRate;
                numStations = numStations;
                @display("p=80,80,ring");
        };
    connections:
        comp[numStations-1].out --> {  delay = cableDelay; } --> comp[0].in;

        for i=0..numStations-2 {
            comp[i].out --> {  delay = cableDelay; } --> comp[i+1].in;
        };
}


