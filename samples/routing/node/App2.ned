//
// This file is part of an OMNeT++/OMNEST simulation example.
//
// Copyright (C) 1992-2015 Andras Varga
//
// This file is distributed WITHOUT ANY WARRANTY. See the file
// `license' for details on this and other legal matters.
//

package node;

//TODO implement App that uses array of dict to control sessions! { startTime, endTime, iaTimes, packetLength, etc. }

//
// Application model to generate traffic for the network.
//
simple App2 like IApp
{
    parameters:
        int address;  // local node address
        string destAddresses;  // destination addresses
        volatile object iaTimes = default([]);
        bool cycleIaTimes = default(false); // when all values in iaTimes are used up, read iaTimes again for another array?
        volatile int packetLength @unit(byte);  // length of one message
        @display("i=block/browser");
        @signal[endToEndDelay](type="simtime_t");
        @signal[hopCount](type="long");
        @signal[sourceAddress](type="long");
        @statistic[endToEndDelay](title="end-to-end delay of arrived packets";unit=s;record=vector,mean,max;interpolationmode=none);
        @statistic[hopCount](title="hop count of arrived packets";interpolationmode=none;record=vector?,mean,max);
        @statistic[sourceAddress](title="source address of arrived packets";interpolationmode=none;record=vector?);
    gates:
        input in;
        output out;
}


