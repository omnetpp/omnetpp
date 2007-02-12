//
// This file is part of an OMNeT++/OMNEST simulation test.
//
// Copyright (C) 1992-2005 Andras Varga
//
// This file is distributed WITHOUT ANY WARRANTY. See the file
// `license' for details on this and other legal matters.
//

simple StressSource
{
    parameters:
        volatile double serviceTime;
        @display("i=block/source");
    gates:
        input directIn;
        output out[];
}

simple StressNode
{
    parameters:
        volatile int outGateIndex = intuniform(0, sizeof(out) - 1);
        @display("i=block/cogwheel");
    gates:
        input directIn;
        input in[];
        output out[];
}

simple StressDirect
{
    parameters:
        volatile double propagationDelay;
        volatile double transmissionDelay;
        @display("i=block/rightarrow");
    gates:
        input directIn;
        input in[];
}

simple StressCapsulate
{
    parameters:
        double encapsulateProbability;
        double decapsulateProbability;
        @display("i=block/socket");
    gates:
        input directIn;
        input in[];
        output out[];
}

simple StressDuplicate
{
    parameters:
        volatile double numberOfDuplicates;
        @display("i=block/fork");
    gates:
        input directIn;
        input in[];
        output out[];
}

simple StressQueue
{
    parameters:
        volatile double serviceTime;
        @display("i=block/queue");
    gates:
        input directIn;
        input in[];
        output out[];
}

simple StressSink
{
    parameters:
        @display("i=block/sink");
    gates:
        input directIn;
        input in[];
}

module StressLossy
{
    parameters:
        double dropProbability;
        @display("i=block/routing");
    gates:
        input in[];
        output out[];
    submodules:
        sink: StressSink;
        node1: StressNode {
            parameters:
                outGateIndex = uniform(0, 1) < dropProbability ? 0 : 1;
        };
        node2: StressNode;
    connections allowunconnected:
        node1.out++ --> sink.in++;
        node1.out++ --> node2.in++;
        for i=1..sizeof(out) {
            node2.out++ --> out++;
        };
        for i=1..sizeof(in) {
            in++ --> node1.in++;
        };
}

network SimpleStress
{
    @display("bgb=801.0,629.0");
    submodules:
        source1: StressSource;
        source2: StressSource;
        capsulate1: StressCapsulate;
        capsulate2: StressCapsulate;
        duplicate1: StressDuplicate;
        duplicate2: StressDuplicate;
        queue1: StressQueue;
        queue2: StressQueue;
        direct1: StressDirect;
        direct2: StressDirect;
        lossy1: StressLossy;
        lossy2: StressLossy;
        sink1: StressSink;
        sink2: StressSink;
    connections allowunconnected:
        source1.out++ --> duplicate1.in++;
        duplicate1.out++ --> capsulate1.in++;
        capsulate1.out++ --> queue1.in++;
        queue1.out++ --> lossy1.in++;
        lossy1.out++ --> sink1.in++;
        source2.out++ --> duplicate2.in++;
        duplicate2.out++ --> capsulate2.in++;
        capsulate2.out++ --> queue2.in++;
        queue2.out++ --> lossy2.in++;
        lossy2.out++ --> sink2.in++;
        source2.out++ --> direct2.in++;
        duplicate2.out++ --> direct2.in++;
        capsulate2.out++ --> direct2.in++;
        queue2.out++ --> direct2.in++;
        lossy2.out++ --> direct2.in++;
        source1.out++ --> direct1.in++;
        duplicate1.out++ --> direct1.in++;
        capsulate1.out++ --> direct1.in++;
        queue1.out++ --> direct1.in++;
        lossy1.out++ --> direct1.in++;
        duplicate1.out++ --> duplicate2.in++;
        duplicate2.out++ --> duplicate1.in++;
        capsulate1.out++ --> capsulate2.in++;
        capsulate2.out++ --> capsulate1.in++;
        queue1.out++ --> queue2.in++;
        queue2.out++ --> queue1.in++;
        lossy1.out++ --> lossy2.in++;
        lossy2.out++ --> lossy1.in++;
        queue1.out++ --> sink1.in++;
        capsulate1.out++ --> sink1.in++;
        duplicate1.out++ --> sink1.in++;
        queue2.out++ --> sink2.in++;
        capsulate2.out++ --> sink2.in++;
        duplicate2.out++ --> sink2.in++;
}
