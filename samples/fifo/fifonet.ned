//
// This file is part of an OMNeT++/OMNEST simulation example.
//
// Copyright (C) 1992-2005 Andras Varga
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
        gen: FFGenerator {
            parameters:
                @display("p=89,100");
        };
        fifo: FFBitFifo {
            parameters:
                @display("p=209,100");
        };
        sink: FFSink {
            parameters:
                @display("p=329,100");
        };
    connections:
        gen.out --> fifo.in;
        fifo.out --> sink.in;
}


