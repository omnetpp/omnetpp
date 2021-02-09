//
// This file is part of an OMNeT++/OMNEST simulation example.
//
// Copyright (C) 2006-2015 OpenSim Ltd.
//
// This file is distributed WITHOUT ANY WARRANTY. See the file
// `license' for details on this and other legal matters.
//

package org.omnetpp.queueing;

//
// Queue that allocates a resource for processing each job.
// If the resource is not available, the queue is idle.
//
simple ResourceBasedQueue
{
    parameters:
        @group(Queueing);
        @signal[dropped](type="long");
        @signal[queueLength](type="long");
        @signal[queueingTime](type="simtime_t");
        @signal[busy](type="bool");
        @statistic[dropped](title="drop event";record=vector?,count;interpolationmode=none);
        @statistic[queueLength](title="queue length";record=vector,timeavg,max;interpolationmode=sample-hold);
        @statistic[queueingTime](title="queueing time at dequeue";record=vector?,mean,max;unit=s;interpolationmode=none);
        @statistic[busy](title="server busy state";record=vector?,timeavg;interpolationmode=sample-hold);
        @display("i=block/activeq;q=queue");

        int capacity = default(-1);    // negative capacity means unlimited queue
        bool fifo = default(true);     // whether the module works as a queue (fifo=true) or a stack (fifo=false)
        volatile double serviceTime @unit(s);

        string resourceModuleName = default("");  // the ResourcePool block to allocate from (absolute module path or relative to the parent module in ".modulename" form)
        int resourceAmount = default(1);          // the amount of resource needed for servicing a job
        int resourcePriority = default(0);        // the priority used during resource allocation (smaller number means higher priority)

    gates:
        input in[];
        output out;
}
