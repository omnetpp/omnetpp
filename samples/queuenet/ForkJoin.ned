//
// This file is part of an OMNeT++/OMNEST simulation example.
//
// Copyright (C) 2006-2015 OpenSim Ltd
//
// This file is distributed WITHOUT ANY WARRANTY. See the file
// `license' for details on this and other legal matters.
//

import org.omnetpp.queueing.Delay;
import org.omnetpp.queueing.Fork;
import org.omnetpp.queueing.JobList;
import org.omnetpp.queueing.Join;
import org.omnetpp.queueing.Sink;
import org.omnetpp.queueing.Source;


//
// Demonstrates the use of the Fork and Join modules.
//
network ForkJoin
{
    submodules:
        source: Source {
            @display("p=54,137");
        }
        fork: Fork {
            @display("p=133,138");
        }
        delay: Delay {
            @display("p=234,47");
        }
        delay1: Delay {
            @display("p=234,138");
        }
        delay2: Delay {
            @display("p=234,242");
        }
        join: Join {
            @display("p=342,137");
        }
        sink: Sink {
            @display("p=418,137");
        }
        jobList: JobList {
            @display("p=382,230");
        }
    connections:
        source.out --> fork.in;
        fork.out++ --> delay.in++;
        delay.out --> join.in++;
        fork.out++ --> delay1.in++;
        delay1.out --> join.in++;
        fork.out++ --> delay2.in++;
        delay2.out --> join.in++;
        join.out --> sink.in++;
}
