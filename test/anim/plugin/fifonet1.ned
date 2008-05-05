//
// generator + FIFO + sink
//
module FifoNet1
{
    parameters:
        volatile double serviceRate1;
        volatile double serviceRate2;
    submodules:
        gen1: FF1Generator {
            parameters:
                messageKind = 1;
                arrivalRate = 100;
                @display("p=50,53;i=gen");
        };
        gen2: FF1Generator {
            parameters:
                messageKind = 2;
                arrivalRate = 100;
                @display("p=50,113;i=gen");
        };
        fifo1: FF1PacketFifo {
            parameters:
                serviceRate = serviceRate1;
                @display("p=165,80;i=queue;q=queue");
            gates:
                in[2];
        };
        fifo2: FF1PacketFifo {
            parameters:
                serviceRate = serviceRate2;
                @display("p=258,80;i=queue;q=queue");
            gates:
                in[1];
        };
        sink: FF1Sink {
            parameters:
                @display("p=345,80;i=sink");
        };
    connections:
        gen1.out --> fifo1.in[0];
        fifo2.out --> sink.in;
        gen2.out --> fifo1.in[1];
        fifo1.out --> fifo2.in[0];
}

network fifonet1 extends FifoNet1
{
    parameters:
        serviceRate1 = 220;
        serviceRate2 = 220;
}

