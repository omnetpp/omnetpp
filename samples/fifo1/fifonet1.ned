//
// This file is part of an OMNeT++ simulation example.
//
// Copyright (C) 1992-2003 Andras Varga
//
// This file is distributed WITHOUT ANY WARRANTY. See the file
// `license' for details on this and other legal matters.
//


import
    "gen1",
    "fifo1",
    "sink1";


//
// Simple queueing network: generator + FIFO + sink.
//
module FifoNet1
    submodules:
        gen: FF1Generator;
            parameters:
                num_messages = input,
                ia_time = input,
                msg_length = input;
            display: "p=76,78;i=gen";
        fifo: FF1BitFifo;
            parameters:
                bits_per_sec = input;
            display: "p=164,78;i=queue;q=queue";
        sink: FF1Sink;
            display: "p=244,78;i=sink";
    connections:
        gen.out --> fifo.in;
        fifo.out --> sink.in;
endmodule

network fifonet1 : FifoNet1
endnetwork

