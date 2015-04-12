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
network FifoNet
{
    submodules:
        gen: Source {
            parameters:
                @display("p=89,100");
        }
        fifo: Fifo {
            parameters:
                @display("p=209,100");
        }
        sink: Sink {
            parameters:
                @display("p=329,100");
        }
    connections:
        gen.out --> fifo.in;
        fifo.out --> sink.in;
}


