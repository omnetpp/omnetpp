//-------------------------------------------------------------
// file: fifonet.ned
//        (part of Fifo2 - an OMNeT++ demo simulation)
//-------------------------------------------------------------


import
    "gen",
    "fifo",
    "sink";


// FifoNet --
//
// generator + FIFO + sink
//
module FifoNet
    submodules:
        gen: FF2BurstyGenerator;
            parameters:
                sleep_time_mean = input,
                burst_time_mean = input,
                send_ia_time = input,
                msg_length = input;
        fifo: ACBFifo;
            parameters:
                bits_per_sec = input;
        sink: FF2Sink;
    connections:
        gen.out --> fifo.in;
        fifo.out --> sink.in;
endmodule

network fifonet : FifoNet
endnetwork


