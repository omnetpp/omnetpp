//
// This file is part of an OMNeT++/OMNEST simulation example.
//
// Copyright (C) 1992-2015 Andras Varga
//
// This file is distributed WITHOUT ANY WARRANTY. See the file
// `license' for details on this and other legal matters.
//

package node;

//
// Bursty message source.
//
simple BurstyApp like IApp
{
    parameters:
        int address;  // local node address
        string destAddresses;  // destination addresses
        volatile double sleepTime @unit(s) = default(30s); // sleep time between bursts
        volatile double burstTime @unit(s) = default(10s); // duration of a burst
        volatile double sendIaTime @unit(s) = default(exponential(1s)); // time between generating packets during a burst
        volatile int packetLength @unit(byte); // length of a message
        @display("i=block/source");
        @signal[endToEndDelay](type="simtime_t");
        @signal[hopCount](type="long");
        @signal[sourceAddress](type="long");
        @statistic[endToEndDelay](title="end-to-end delay of arrived packets";unit=s;interpolationmode=none);
        @statistic[hopCount](title="hop count of arrived packets";interpolationmode=none);
        @statistic[sourceAddress](title="source address of arrived packets";interpolationmode=none);
    gates:
        output out;
        input in;
}


