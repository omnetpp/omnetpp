//-------------------------------------------------------------
// file: fifonet.ned
//        (part of Fifo1 - an OMNeT++ demo simulation)
//-------------------------------------------------------------

include "gen", "fifo", "sink";

module FifoNet
  submodules:
    gen: Generator
      parameters:
        num_messages = input,
        ia_time = input,
        msg_length = input;
    fifo: ACBFifo
      parameters:
        bits_per_sec = input;
    sink: Sink;
  connections:
    gen.out --> fifo.in;
    fifo.out --> sink.in;
endmodule

network fifonet : FifoNet
endnetwork
