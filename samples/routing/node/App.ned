//
// This file is part of an OMNeT++/OMNEST simulation example.
//
// Copyright (C) 1992-2005 Andras Varga
//
// This file is distributed WITHOUT ANY WARRANTY. See the file
// `license' for details on this and other legal matters.
//

package node;

//
// Application model to generate traffic for the network.
//
simple App
{
    parameters:
        int address;  // local node address
        volatile double sendIaTime @unit(s); // time between generating packets
        int packetLength @unit(byte);  // length of one message
        string destAddresses;  // destination addresses
        @display("i=block/browser");
    gates:
        input in;
        output out;
}


