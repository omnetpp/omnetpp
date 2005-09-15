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
    parameters:
        // Number of stations. Destination MAC address will be a random
        // [0,numStatons-1] integer, omitting our own address.
        numStations : numeric const,
        // MAC Address of this station.
        address : numeric const,
        // Number of messages to generate.
        numMessages : numeric const,
        // Message length in bytes. It can be a random variate.
        messageLength : numeric,
        // Time between two messages. It can be a random variate.
        interArrivalTime : numeric;
    gates:
        out: out;
endsimple

