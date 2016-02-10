//
// This file is part of an OMNeT++/OMNEST simulation example.
//
// Copyright (C) 2006-2015 OpenSim Ltd
//
// This file is distributed WITHOUT ANY WARRANTY. See the file
// `license' for details on this and other legal matters.
//

import org.omnetpp.queueing.Queue;
import org.omnetpp.queueing.Sink;
import org.omnetpp.queueing.Source;


//
// This simple queueing network only contains a source, a FIFO queue and a sink.
//
network SimpleQueueExt
{
    parameters:
        @display("i=block/network2;bgb=371,217");
    submodules:
        sink: SinkExt {
            @display("p=293,127");
        }
        queue: Queue {
            @display("p=165.0,79.0");
        }
        source: Source {
            @display("p=50.0,79.0");
        }
    connections:
        source.out --> queue.in++;
        queue.out --> sink.in++;
}
