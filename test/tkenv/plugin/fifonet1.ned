

import
    "gen1",
    "fifo1",
    "sink1";



// FifoNet1 --
//
// generator + FIFO + sink
//
module FifoNet1
    parameters:
        serviceRate1: numeric,
        serviceRate2: numeric;
    submodules:
        gen1: FF1Generator;
            parameters:
                messageKind = 1,
                arrivalRate = 100;
            display: "p=50,53;i=gen";
        gen2: FF1Generator;
            parameters:
                messageKind = 2,
                arrivalRate = 100;
            display: "p=50,113;i=gen";
        fifo1: FF1PacketFifo;
            parameters:
                serviceRate = ref serviceRate1;
            gatesizes:
                in[2];
            display: "p=165,80;i=queue;q=queue";
        fifo2: FF1PacketFifo;
            parameters:
                serviceRate = ref serviceRate2;
            gatesizes:
                in[1];
            display: "p=258,80;i=queue;q=queue";
        sink: FF1Sink;
            display: "p=345,80;i=sink";
    connections:
        gen1.out --> fifo1.in[0];
        fifo2.out --> sink.in;
        gen2.out --> fifo1.in[1];
        fifo1.out --> fifo2.in[0];
endmodule

network fifonet1 : FifoNet1
    parameters:
        serviceRate1 = 220,
        serviceRate2 = 220;
endnetwork

