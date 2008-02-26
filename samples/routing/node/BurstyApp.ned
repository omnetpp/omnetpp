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
// Bursty message source.
//
simple BurstyApp
{
    parameters:
        int address;  // local node address
        volatile double sleepTime @unit(s); // sleep time between bursts
        volatile double burstTime @unit(s); // duration of a burst
        volatile double sendIaTime @unit(s); // time between generating packets during a burst
        volatile double packetLength @unit(byte); // length of a message
        @display("i=block/source");
    gates:
        output out;
        input in;
}


