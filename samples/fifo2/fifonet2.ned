//
// This file is part of an OMNeT++ simulation example.
//
// Copyright (C) 1992-2003 Andras Varga
//
// This file is distributed WITHOUT ANY WARRANTY. See the file
// `license' for details on this and other legal matters.
//


import
    "gen2",
    "fifo2",
    "sink2";


//
// Simple queueing network: generator + FIFO + sink.
//
module FifoNet2
    submodules:
        gen: FF2BurstyGenerator;
            parameters:
                sleep_time_mean = input,
                burst_time_mean = input,
                send_ia_time = input,
                msg_length = input;
            display: "p=76,68;i=gen";
        fifo: FF2BitFifo;
            parameters:
                bits_per_sec = input;
            display: "p=164,68;i=queue;q=queue";
        sink: FF2Sink;
            display: "p=252,68;i=sink";
    connections:
        gen.out --> fifo.in;
        fifo.out --> sink.in;
endmodule


network fifonet2 : FifoNet2
endnetwork

