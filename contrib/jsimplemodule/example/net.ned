
module Net1
    submodules:
        tic: JSimpleModule;
            parameters:
                javaClass = "Txc";
            display: "i=misc/node,blue";
        toc: JSimpleModule;
            parameters:
                javaClass = "Txc";
            display: "i=misc/node,yellow";
    connections:
        tic.out++ --> toc.in++;
        tic.in++ <-- toc.out++;
endmodule

network net1 : Net1
endnetwork


module Net2
    submodules:
        src: JSimpleModule;
            parameters:
                javaClass = "Source";
            display: "i=block/source";
        sink: JSimpleModule;
            parameters:
                javaClass = "Sink";
            display: "i=block/sink";
    connections:
        src.out++ --> sink.in++;
endmodule

network net2 : Net2
endnetwork

module Net3
    parameters:
        queueNumInitialJobs: numeric const,
        queueServiceTime: numeric;
    submodules:
        src: JSimpleModule;
            parameters:
                javaClass = "Source";
            display: "i=block/source";
        fifo: JSimpleModule;
            parameters:
                javaClass = "Fifo";
            display: "i=block/queue;q=queue";
        sink: JSimpleModule;
            parameters:
                javaClass = "Sink";
            display: "i=block/sink";
    connections:
        src.out++ --> fifo.in++;
        fifo.out++ --> sink.in++;
endmodule

network net3 : Net3
    parameters:
        queueNumInitialJobs = 5,
        queueServiceTime = exponential(1);
endnetwork

module Net4
    parameters:
        numQueues: numeric const,
        queueNumInitialJobs: numeric const,
        queueServiceTime: numeric;
    submodules:
        src: JSimpleModule;
            parameters:
                javaClass = "Source";
            display: "i=block/source";
        fork: JSimpleModule;
            parameters:
                javaClass = "Fork";
            display: "i=block/fork";
        fifo: JSimpleModule[numQueues];
            parameters:
                javaClass = "Fifo";
            display: "i=block/queue;q=queue;p=,,column";
        join: JSimpleModule;
            parameters:
                javaClass = "Join";
            display: "i=block/join";
        sink: JSimpleModule;
            parameters:
                javaClass = "Sink";
            display: "i=block/sink";
    connections:
        src.out++ --> fork.in++;

        for i=0..numQueues-1 do
            fork.out++ --> fifo[i].in++;
            fifo[i].out++ --> join.in++;
        endfor;

        join.out++ --> sink.in++;
endmodule

network net4 : Net4
    parameters:
        numQueues = 3,
        queueNumInitialJobs = 0,
        queueServiceTime = exponential(1);
endnetwork

