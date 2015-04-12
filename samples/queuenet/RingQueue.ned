//
// This file is part of an OMNeT++/OMNEST simulation example.
//
// Copyright (C) 2006-2015 OpenSim Ltd
//
// This file is distributed WITHOUT ANY WARRANTY. See the file
// `license' for details on this and other legal matters.
//

import org.omnetpp.queueing.Queue;
import org.omnetpp.queueing.Source;


//
// This is a closed ring network.
// Based on the idea from Nicky van Foreest (vanforeest@gmail.com)
//
network RingQueue
{
    parameters:
        @display("i=device/lan-ring");
    submodules:
        source1: Source {
            @display("p=370.0,36.0");
        }
        source: Source {
            @display("p=54.0,45.0");
        }
        queue: Queue {
            @display("p=110.0,124.0");
        }
        queue1: Queue {
            @display("p=139.0,241.0");
        }
        queue2: Queue {
            @display("p=282.0,251.0");
        }
        queue3: Queue {
            @display("p=327.0,134.0");
        }
        queue4: Queue {
            @display("p=232.0,64.0");
        }
    connections:
        queue4.out --> queue.in++;
        queue.out --> queue1.in++;
        queue1.out --> queue2.in++;
        queue2.out --> queue3.in++;
        queue3.out --> queue4.in++;
        source.out --> queue.in++;
        source1.out --> queue3.in++;
}
