
module Net1
    submodules:
        tic: JSimpleModule;
            parameters:
                javaClass = "Txc";
        toc: JSimpleModule;
            parameters:
                javaClass = "Txc";
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
