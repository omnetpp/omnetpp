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
// A resource object representing a single type of resource.
// More than one ResourceBasedQueue block can specify this block as a resource.
// In this case unstatisfied resource requests are queued (with priority) and the
// requestor will be notified once the resource gets available.
//
simple ResourcePool
{
    parameters:
        @group(Queueing);
        @signal[amount](type="long");
        @statistic[amount](title="available resource";record=vector?,timeavg,min;interpolationmode=sample-hold);
        @display("i=block/buffer;t=,r,black");
        int amount = default(1);        // the amount of resource units
}
