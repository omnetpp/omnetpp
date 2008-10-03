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
// A resource object representing a single type of resource.
// More tan one ResourceBasedQueue block can specify this bolck as a resource.
// In this case unstatisfied resource requests are queued (with priority) and the
// requestor will be notified once the resource gets available.
//
simple ResourcePool
{
    parameters:
        @group(Queueing);
        @display("i=block/buffer;t=,r,black");
        int amount = default(1);        // the amount of resource units
}
