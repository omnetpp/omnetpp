//
// This file is part of an OMNeT++/OMNEST simulation example.
//
// Copyright (C) 2006-2015 OpenSim Ltd
//
// This file is distributed WITHOUT ANY WARRANTY. See the file
// `license' for details on this and other legal matters.
//

import org.omnetpp.queueing.Allocate;
import org.omnetpp.queueing.Deallocate;
import org.omnetpp.queueing.Delay;
import org.omnetpp.queueing.ResourcePool;
import org.omnetpp.queueing.Sink;
import org.omnetpp.queueing.Source;


//
// Resource allocation-deallocation example
//
network AllocDealloc
{
    submodules:
        source: Source {
            @display("p=50,82");
        }
        allocate: Allocate {
            @display("p=153,82");
        }
        deallocate: Deallocate {
            @display("p=338,82");
        }
        delay: Delay {
            @display("p=250,82");
        }
        delay1: Delay {
            @display("p=254,164");
        }
        allocate1: Allocate {
            @display("p=157,164");
        }
        deallocate1: Deallocate {
            @display("p=342,164");
        }
        allocate2: Allocate {
            @display("p=157,242");
        }
        deallocate2: Deallocate {
            @display("p=342,242");
        }
        delay2: Delay {
            @display("p=254,242");
        }
        source1: Source {
            @display("p=50,164");
        }
        source2: Source {
            @display("p=50,242");
        }
        sink: Sink {
            @display("p=445,82");
        }
        sink1: Sink {
            @display("p=445,164");
        }
        sink2: Sink {
            @display("p=445,242");
        }
        resourcePool: ResourcePool {
            @display("p=196,321");
        }
    connections:
        source.out --> allocate.in++;
        allocate.out --> delay.in++;
        delay.out --> deallocate.in++;
        source1.out --> allocate1.in++;
        allocate1.out --> delay1.in++;
        delay1.out --> deallocate1.in++;
        source2.out --> allocate2.in++;
        allocate2.out --> delay2.in++;
        delay2.out --> deallocate2.in++;
        deallocate.out --> sink.in++;
        deallocate1.out --> sink1.in++;
        deallocate2.out --> sink2.in++;
}
