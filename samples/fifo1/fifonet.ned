//-------------------------------------------------------------
// file: fifonet.ned
//        (part of Fifo1 - an OMNeT++ demo simulation)
//-------------------------------------------------------------


import
    "gen",
    "fifo",
    "sink";


// FifoNet1 --
//
// generator + FIFO + sink
//
module FifoNet1
    submodules:
        gen: FF1Generator;
            parameters:
                num_messages = input,
                ia_time = input,
                msg_length = input;
        fifo: ACBFifo;
            parameters:
                bits_per_sec = input;
        sink: FF1Sink;
    connections:
        gen.out --> fifo.in;
        fifo.out --> sink.in;
endmodule

network fifonet1 : FifoNet1
endnetwork




