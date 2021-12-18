//
// This file is part of an OMNeT++/OMNEST simulation example.
//
// Copyright (C) 1992-2015 Andras Varga
//
// This file is distributed WITHOUT ANY WARRANTY. See the file
// `license' for details on this and other legal matters.
//

//
// Simple queueing network: generator + FIFO + sink.
//
network SingleQueue
{
    submodules:
        source: Source {
            @display("p=100,100");
        }
        fifo: Fifo {
            @display("p=200,100");
        }
        sink: Sink {
            @display("p=300,100");
        }
    connections:
        source.out --> fifo.in;
        fifo.out --> sink.in;
}


