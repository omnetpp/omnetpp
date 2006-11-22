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
// Implements the Token Ring protocol.
//
simple TokenRingMAC
{
    parameters:
        // Token Ring data rate in bps. Standard values are 4 or 16 Mbps.
        double dataRate;
        // Token Holding Time, as defined in the protocol.
        double THT;
        // MAC address of this station, modeled with an integer.
        double address;
        // Max queue size in packets.
        double queueMaxLen;
    gates:
        input phy_in;
        input from_hl;
        output phy_out;
        output to_hl;
}
