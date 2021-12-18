//
// This file is part of an OMNeT++/OMNEST simulation example.
//
// Copyright (C) 2006-2015 OpenSim Ltd
//
// This file is distributed WITHOUT ANY WARRANTY. See the file
// `license' for details on this and other legal matters.
//

//
// This is queueing network consists of several queues, connected in tandem.
//
network TandemQueues
{
    parameters:
        @display("i=block/network");
    submodules:
        source: Source {
            @display("p=100,100");
        }
        fifo1: Fifo {
            @display("p=200,100");
        }
        fifo2: Fifo {
            @display("p=300,100");
        }
        sink: Sink {
            @display("p=400,100");
        }
    connections:
        source.out --> fifo1.in;
        fifo1.out --> fifo2.in;
        fifo2.out --> sink.in;
}
