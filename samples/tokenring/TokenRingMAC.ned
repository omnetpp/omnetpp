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
// Implements the Token Ring protocol.
//
simple TokenRingMAC
{
    parameters:
        double tokenHoldingTime @unit(s); // Token Holding Time, as defined in the protocol.
        int address; // MAC address of this station, modeled with an integer.
        int queueMaxLen; // max queue size in packets.
        @display("i=block/queue;q=sendQueue");
    gates:
        input phyIn;
        output phyOut;
        input fromHL;
        output toHL;
}
