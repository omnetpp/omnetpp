//
// This file is part of an OMNeT++/OMNEST simulation test.
//
// Copyright (C) 1992-2015 Andras Varga
//
// This file is distributed WITHOUT ANY WARRANTY. See the file
// `license' for details on this and other legal matters.
//

simple StressSource
{
    parameters:
        volatile double serviceTime @unit(s);
        volatile int messageLength @unit(byte);
        @display("i=block/source");
    gates:
        input directIn;
        output out[];
}

simple StressNode
{
    parameters:
        volatile int outGateIndex = default(intuniform(0, sizeof(out) - 1));
        @display("i=block/cogwheel");
    gates:
        input directIn;
        input in[];
        output out[];
}

simple StressDirect
{
    parameters:
        volatile double propagationDelay @unit(s);
        volatile double transmissionDelay @unit(s);
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
        volatile int messageLength @unit(byte);
        @display("i=block/socket");
    gates:
        input directIn;
        input in[];
        output out[];
}

simple StressDuplicate
{
    parameters:
        volatile int numberOfDuplicates;
        @display("i=block/fork");
    gates:
        input directIn;
        input in[];
        output out[];
}

simple StressQueue
{
    parameters:
        @display("i=block/activeq");
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

channel StressChannel extends ned.DatarateChannel
{
    @class(StressChannel);
}

module StressLossy
{
    parameters:
        double dropProbability;
        @display("i=block/routing");
    gates:
        input directIn;
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
        node1.out++ --> StressChannel --> sink.in++;
        node1.out++ --> StressChannel --> node2.in++;
        for i=1..sizeof(out) {
            node2.out++ --> out++;
        };
        for i=1..sizeof(in) {
            in++ --> node1.in++;
        };
        directIn --> node1.in++;
}

network SimpleStress
{
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
        source1.out++ --> StressChannel --> duplicate1.in++;
        duplicate1.out++ --> StressChannel --> capsulate1.in++;
        capsulate1.out++ --> StressChannel --> queue1.in++;
        queue1.out++ --> StressChannel --> lossy1.in++;
        lossy1.out++ --> StressChannel --> sink1.in++;
        source2.out++ --> StressChannel --> duplicate2.in++;
        duplicate2.out++ --> StressChannel --> capsulate2.in++;
        capsulate2.out++ --> StressChannel --> queue2.in++;
        queue2.out++ --> StressChannel --> lossy2.in++;
        lossy2.out++ --> StressChannel --> sink2.in++;
        source2.out++ --> StressChannel --> direct2.in++;
        duplicate2.out++ --> StressChannel --> direct2.in++;
        capsulate2.out++ --> StressChannel --> direct2.in++;
        queue2.out++ --> StressChannel --> direct2.in++;
        lossy2.out++ --> StressChannel --> direct2.in++;
        source1.out++ --> StressChannel --> direct1.in++;
        duplicate1.out++ --> StressChannel --> direct1.in++;
        capsulate1.out++ --> StressChannel --> direct1.in++;
        queue1.out++ --> StressChannel --> direct1.in++;
        lossy1.out++ --> StressChannel --> direct1.in++;
        duplicate1.out++ --> StressChannel --> duplicate2.in++;
        duplicate2.out++ --> StressChannel --> duplicate1.in++;
        capsulate1.out++ --> StressChannel --> capsulate2.in++;
        capsulate2.out++ --> StressChannel --> capsulate1.in++;
        queue1.out++ --> StressChannel --> queue2.in++;
        queue2.out++ --> StressChannel --> queue1.in++;
        lossy1.out++ --> StressChannel --> lossy2.in++;
        lossy2.out++ --> StressChannel --> lossy1.in++;
        queue1.out++ --> StressChannel --> sink1.in++;
        capsulate1.out++ --> StressChannel --> sink1.in++;
        duplicate1.out++ --> StressChannel --> sink1.in++;
        queue2.out++ --> StressChannel --> sink2.in++;
        capsulate2.out++ --> StressChannel --> sink2.in++;
        duplicate2.out++ --> StressChannel --> sink2.in++;
}

network LineStress
{
    submodules:
        source: StressSource;
        node[6]: StressNode;
        sink: StressSink;
    connections allowunconnected:
        source.out++ --> StressChannel --> node[0].in++;

        node[0].out++ --> StressChannel --> node[1].in++;
        node[1].out++ --> StressChannel --> node[2].in++;
        node[2].out++ --> StressChannel --> node[3].in++;
        node[3].out++ --> StressChannel --> node[4].in++;
        node[4].out++ --> StressChannel --> node[5].in++;

        node[1].out++ --> StressChannel --> node[0].in++;
        node[2].out++ --> StressChannel --> node[1].in++;
        node[3].out++ --> StressChannel --> node[2].in++;
        node[4].out++ --> StressChannel --> node[3].in++;
        node[5].out++ --> StressChannel --> node[4].in++;

        node[5].out++ --> StressChannel --> sink.in++;
}

network CircleStress
{
    submodules:
        source: StressSource;
        node[6]: StressNode;
        sink: StressSink;
    connections allowunconnected:
        source.out++ --> node[0].in++;

        node[0].out++ --> StressChannel --> node[1].in++;
        node[1].out++ --> StressChannel --> node[2].in++;
        node[2].out++ --> StressChannel --> node[3].in++;
        node[3].out++ --> StressChannel --> node[4].in++;
        node[4].out++ --> StressChannel --> node[5].in++;
        node[5].out++ --> StressChannel --> node[0].in++;

        node[0].out++ --> StressChannel --> node[5].in++;
        node[1].out++ --> StressChannel --> node[0].in++;
        node[2].out++ --> StressChannel --> node[1].in++;
        node[3].out++ --> StressChannel --> node[2].in++;
        node[4].out++ --> StressChannel --> node[3].in++;
        node[5].out++ --> StressChannel --> node[4].in++;

        node[3].out++ --> StressChannel --> sink.in++;
}
