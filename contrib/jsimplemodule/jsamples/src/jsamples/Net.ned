package jsamples;

import JSimpleModule;

network Net1
{
    submodules:
        tic: Txc;
        toc: Txc;
    connections:
        tic.out --> { delay = 1ms; } --> toc.in;
        tic.in <-- { delay = 1ms; } <-- toc.out;
}

network Net2
{
    submodules:
        src: Source;
        sink: Sink;
    connections:
        src.out --> sink.in;
}

network Net3
{
    parameters:
        double queueNumInitialJobs;
        volatile double queueServiceTime @unit(s);
    submodules:
        src: Source;
        fifo: Fifo;
        sink: Sink;
    connections:
        src.out --> fifo.in;
        fifo.out --> sink.in;
}

network Net4
{
    parameters:
        double numQueues;
        double queueNumInitialJobs;
        volatile double queueServiceTime @unit(s);
    submodules:
        src: Source;
        fork: Fork;
        fifo[numQueues]: Fifo;
        join: Join;
        count: CountHellos;
        sink: Sink;
    connections:
        src.out --> fork.in;

        for i=0..numQueues-1 {
            fork.out++ --> fifo[i].in;
            fifo[i].out --> join.in++;
        }

        join.out --> count.in;
        count.out --> sink.in;
}

network Net5
{
    submodules:
        node1: Node;
        node2: Node;
    connections:
        node1.out --> node2.in;
        node1.in <-- node2.out;
}
