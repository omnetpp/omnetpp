//
// This file is part of an OMNeT++/OMNEST simulation example.
//
// Copyright (C) 1992-2005 Andras Varga
//
// This file is distributed WITHOUT ANY WARRANTY. See the file
// `license' for details on this and other legal matters.
//


import
    "gen",
    "fifo",
    "sink";


//
// Simple queueing network: generator + FIFO + sink.
//
module FifoNet
    submodules:
        gen: FFGenerator;
            display: "p=89,100;i=block/source";
        fifo: FFBitFifo;
            display: "p=209,100;i=block/queue;q=queue";
        sink: FFSink;
            display: "p=329,100;i=block/sink";
    connections:
        gen.out --> fifo.in;
        fifo.out --> sink.in;
endmodule


network fifonet : FifoNet
endnetwork

