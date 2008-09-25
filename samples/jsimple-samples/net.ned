simple Txc extends JSimpleModule
{
    parameters:
        @display("i=misc/node");
    gates:
        input in[];
        output out[];
}

network Net1
{
    submodules:
        tic: Txc;
        toc: Txc;
    connections:
        tic.out++ --> toc.in++;
        tic.in++ <-- toc.out++;
}

simple Source extends JSimpleModule
{
    parameters:
        @display("i=block/source");
    gates:
        output out[];
}

simple Sink extends JSimpleModule
{
    parameters:
        @display("i=block/sink");
    gates:
        input in[];
}

network Net2
{
    submodules:
        src: Source;
        sink: Sink;
    connections:
        src.out++ --> sink.in++;
}

simple Fifo
{
    parameters:
        @display("i=block/queue;q=queue");
    gates:
        input in[];
        output out[];
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
        src.out++ --> fifo.in++;
        fifo.out++ --> sink.in++;
}

simple Fork extends JSimpleModule
{
    parameters:
        @display("i=block/fork");
    gates:
        input in[];
        output out[];
}

simple Join extends JSimpleModule {
    parameters:
        @display("i=block/join");
    gates:
        input in[];
        output out[];
}

simple CountHellos extends JSimpleModule {
    parameters:
        @display("p=441,176;i=block/star");
    gates:
        input in[];
        output out[];
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
        src.out++ --> fork.in++;

        for i=0..numQueues-1 {
            fork.out++ --> fifo[i].in++;
            fifo[i].out++ --> join.in++;
        }

        join.out++ --> count.in++;
        count.out++ --> sink.in++;
}

simple Node extends JSimpleModule {
    parameters:
        @display("i=misc/square_s");
    gates:
        input in[];
        output out[];
}

network Net5
{
    submodules:
        node1: Node;
        node2: Node;
    connections:
        node1.out++ --> node2.in++;
        node1.in++ <-- node2.out++;
}
