//
// This file is part of an OMNeT++/OMNEST simulation example.
//
// Copyright (C) 1992-2008 Andras Varga
//
// This file is distributed WITHOUT ANY WARRANTY. See the file
// `license' for details on this and other legal matters.
//
// Authors: Gabor Lencse and Andras Varga (TU Budapest),
// based on the code from Maurits Andre, George van Montfort,
// and Gerard van de Weerd (TU Delft).
//




//
// A node in a Token Ring LAN. Each instance will be connected to its right neighbour.
//
module Station
{
    parameters:
        int address; // MAC Address of this station.
        int numStations; // Number of stations. MAC addresses are 0..numStations-1 integers
        double dataRate @unit(bps); // Token Ring data rate
        double THT @unit(s); // Token Holding Time, as defined in the protocol.
        @display("i=device/pc4;q=sendQueue");
    gates:
        input in;
        output out;
    submodules:
        mac: TokenRingMAC {
            parameters:
                address = address;
                dataRate = dataRate;
                THT = THT;
                @display("p=137,166");
        }
        gen: Generator {
            parameters:
                numStations = numStations;
                address = address;
                @display("p=79,60");
        }
        sink: Sink {
            parameters:
                @display("p=196,60");
        }
    connections:
        in --> mac.phyIn;
        mac.phyOut --> out;
        mac.toHL --> sink.in;
        gen.out --> mac.fromHL;
}
