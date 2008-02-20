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
// Generates data packets for MAC.
//
simple Generator
{
    parameters:
        int numStations; // number of stations. Destination MAC address will be a random
                         // [0,numStatons-1] integer, omitting our own address.
        int address; // MAC address of this station.
        int numMessages; // number of messages to generate.
        volatile int messageLength; // message length in bytes
        volatile double interArrivalTime @unit(s); // time between two messages
        @display("i=block/source");
    gates:
        output out;
}

