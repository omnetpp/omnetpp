//
// This file is part of an OMNeT++/OMNEST simulation example.
//
// Copyright (C) 1992-2008 Andras Varga
//
// This file is distributed WITHOUT ANY WARRANTY. See the file
// `license' for details on this and other legal matters.
//

import omnetpp.queueing.Delay;
import omnetpp.queueing.JobList;
import omnetpp.queueing.Join;
import omnetpp.queueing.Sink;
import omnetpp.queueing.Source;
import omnetpp.queueing.Split;

//
// Demonstrates the use of the Split and Join modules.
//
network SplitJoin
{
    submodules:
        source: Source {
            @display("p=54,137");
        }
        split: Split {
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
        source.out --> split.in;
        split.out++ --> delay.in++;
        delay.out --> join.in++;
        split.out++ --> delay1.in++;
        delay1.out --> join.in++;
        split.out++ --> delay2.in++;
        delay2.out --> join.in++;
        join.out --> sink.in++;
}
