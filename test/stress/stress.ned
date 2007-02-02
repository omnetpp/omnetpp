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
    submodules:
        direct1: StressDirect;
        direct2: StressDirect;
        lossy: StressLossy;
        source: StressSource;
        capsulate1: StressCapsulate;
        capsulate2: StressCapsulate;
        duplicate1: StressDuplicate;
        duplicate2: StressDuplicate;
        sink: StressSink;
    connections allowunconnected:
        source.out++ --> capsulate2.in++;
        source.out++ --> duplicate1.in++;
        duplicate1.out++ --> capsulate1.in++;
        capsulate1.out++ --> sink.in++;
        capsulate2.out++ --> duplicate2.in++;
        duplicate2.out++ --> sink.in++;
        duplicate1.out++ --> capsulate2.in++;
        duplicate2.out++ --> capsulate1.in++;
        capsulate1.out++ --> duplicate1.in++;
        duplicate2.out++ --> capsulate2.in++;
        capsulate1.out++ --> duplicate2.in++;
        capsulate2.out++ --> duplicate1.in++;
        capsulate1.out++ --> lossy.in++;
        capsulate2.out++ --> lossy.in++;
        duplicate1.out++ --> lossy.in++;
        duplicate2.out++ --> lossy.in++;
        lossy.out++ --> capsulate1.in++;
        lossy.out++ --> duplicate1.in++;
        lossy.out++ --> capsulate2.in++;
        lossy.out++ --> duplicate2.in++;
        capsulate2.out++ --> direct2.in++;
        duplicate2.out++ --> direct2.in++;
        duplicate1.out++ --> direct1.in++;
        capsulate1.out++ --> direct1.in++;
}
