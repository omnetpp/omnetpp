
module Net1
    submodules:
        a: JSimpleModule;
            parameters:
                javaClass = "Tic";
        b: JSimpleModule;
            parameters:
                javaClass = "Tic";
    connections:
        a.out++ --> b.in++;
        a.in++ <-- b.out++;
endmodule

network net1 : Net1
endnetwork


module Net2
    parameters:
        queueNumInitialJobs: numeric const,
        queueServiceTime: numeric;
    submodules:
        src: JSimpleModule;
            parameters:
                javaClass = "Src";
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

network net2 : Net2
    parameters:
        queueNumInitialJobs = 5,
        queueServiceTime = exponential(1);
endnetwork
