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
// A module that allocates a resource for each job. If no resource is
// available, jobs are queued.
//
simple Allocate
{
    parameters:
        @group(Queueing);
        @signal[dropped](type="long");
        @signal[queueLength](type="long");
        @signal[queueingTime](type="simtime_t");
        @statistic[dropped](title="drop event";record=vector?,count;interpolationmode=none);
        @statistic[queueLength](title="queue length";record=vector,timeavg,max;interpolationmode=sample-hold);
        @statistic[queueingTime](title="queueing time at dequeue";record=vector?,mean,max;unit=s;interpolationmode=none);
        @display("i=block/arrival;q=queue");

        int capacity = default(-1);    // negative capacity means unlimited queue
        bool fifo = default(true);     // whether the module works as a queue (fifo=true) or a stack (fifo=false)

        string resourceModuleName;         // the ResourcePool block to allocate from (absolute module path or relative to the parent module in ".modulename" form)
        int resourceAmount = default(1);   // the amount of resource allocated
        int resourcePriority = default(0); // the priority used during resource allocation (smaller number means higher priority)

    gates:
        input in[];
        output out;
}
