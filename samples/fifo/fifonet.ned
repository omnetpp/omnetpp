//
// This file is part of an OMNeT++ simulation example.
//
// Copyright (C) 1992-2003 Andras Varga
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
module FifoNet2
    submodules:
        gen: FFGenerator;
            parameters:
            display: "p=76,68;i=gen";
        fifo: FFBitFifo;
            parameters:
                bitsPerSec = input;
            display: "p=164,68;i=queue;q=queue";
        sink: FFSink;
            display: "p=252,68;i=sink";
    connections:
        gen.out --> fifo.in;
        fifo.out --> sink.in;
endmodule


network fifonet2 : FifoNet2
endnetwork

