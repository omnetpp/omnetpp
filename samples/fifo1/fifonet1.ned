// -------------------------------------------------------------
// file: fifonet1.ned
//        (part of Fifo1 - an OMNeT++ demo simulation)
// -------------------------------------------------------------


import  
    "gen1", 
    "fifo1", 
    "sink1"; 


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
            display: "p=76,78;i=gen";
        fifo: FF1BitFifo; 
            parameters: 
                bits_per_sec = input; 
            display: "p=164,78;i=queue";
        sink: FF1Sink; 
            display: "p=244,78;i=sink";
    connections: 
        gen.out --> fifo.in; 
        fifo.out --> sink.in; 
endmodule 

network fifonet1 : FifoNet1 
endnetwork 

