//
// This file is part of an OMNeT++/OMNEST simulation example.
//
// Copyright (C) 2006-2008 OpenSim Ltd.
//
// This file is distributed WITHOUT ANY WARRANTY. See the file
// `license' for details on this and other legal matters.
//

package org.omnetpp.queueing;

//
// A queue with a built-in server
//
simple Queue
{
    parameters:
        @group(Queueing);
        @display("i=block/queue;q=queue");
        @signal[dropped](title="drop event";modehint=vector,count;interpolationmode=none);
        @signal[queueLength](title="queue length";modehint=vector,timeavg,max;interpolationmode=sample-hold);
        @signal[queueingTime](title="queueing time at dequeue";modehint=vector,mean,max;unit=s;interpolationmode=none);
        @signal[busy](title="server busy state";modehint=vector,timeavg;interpolationmode=sample-hold);

        int capacity = default(-1);    // negative capacity means unlimited queue
        bool fifo = default(true);     // whether the module works as a queue (fifo=true) or a stack (fifo=false)
        volatile double serviceTime @unit(s);
    gates:
        input in[];
        output out;
}
