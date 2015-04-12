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
// Releases a specified amount of resources on the arrival of each job.
// Jobs are sent out on the "out" gate without any change.
//
simple Deallocate
{
    parameters:
        @display("i=block/departure");
        string resourceModuleName;         // the ResourcePool block to release resources to (absolute module path or relative to the parent module in ".modulename" form)
        int resourceAmount = default(1);   // the amount of resource to deallocate
    gates:
        input in[];
        output out;
}
