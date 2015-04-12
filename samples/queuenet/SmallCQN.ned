//
// This file is part of an OMNeT++/OMNEST simulation example.
//
// Copyright (C) 2006-2015 OpenSim Ltd
//
// This file is distributed WITHOUT ANY WARRANTY. See the file
// `license' for details on this and other legal matters.
//

import org.omnetpp.queueing.Dot;
import org.omnetpp.queueing.Queue;
import org.omnetpp.queueing.SourceOnce;

//
// Two queues in a closed queueing network
//
network SmallCQN
{
    submodules:
        queue1: Queue {
            @display("p=137,68");
        }
        queue2: Queue {
            @display("p=137,235");
        }
        dot2: Dot {
            @display("p=217,156");
        }
        dot1: Dot {
            @display("p=57,156");
        }
        sourceOnce: SourceOnce {
            @display("p=73,37;is=s");
            jobName = "job";
            numJobs = 10;
        }
    connections:
        queue1.out --> dot2.in;
        dot2.out --> queue2.in++;
        queue2.out --> dot1.in;
        dot1.out --> queue1.in++;
        sourceOnce.out --> queue1.in++;
}
