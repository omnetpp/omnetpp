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
        @signal[dropped](title="drop event";modeHint=count;interpolationMode=none);
        @signal[queueLength](title="queue length";modeHint=timeavg,max;interpolationMode=sample-hold);
        @signal[queueingTime](title="queueing time at dequeue";modeHint=mean,max;unit=s;interpolationMode=none);
        @signal[busy](title="server busy state";modeHint=timeavg;interpolationMode=sample-hold);
        
        int capacity = default(-1);    // negative capacity means unlimited queue
        bool fifo = default(true);     // whether the module works as a queue (fifo=true) or a stack (fifo=false)
        volatile double serviceTime @unit(s);
    gates:
        input in[];
        output out;
}
