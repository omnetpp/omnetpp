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
        input in[];
        output out[];
}

simple StressCapsulate
{
    parameters:
        double encapsulateProbability;
        double decapsulateProbability;
        @display("i=block/socket");
    gates:
        input in[];
        output out[];
}

simple StressDuplicate
{
    parameters:
        volatile double numberOfDuplicates;
        @display("i=block/fork");
    gates:
        input in[];
        output out[];
}

simple StressSink
{
    parameters:
        @display("i=block/sink");
    gates:
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
        sink: StressSink {
            @display("p=200.0,154.0");
        };
        node1: StressNode {
            parameters:
                outGateIndex = uniform(0, 1) < dropProbability ? 0 : 1;
                @display("p=125.0,64.0");
        };
        node2: StressNode {
            @display("p=55.0,152.0");
        };
    connections:
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
        lossy: StressLossy {
            @display("p=329.0,171.0");
        };
        source: StressSource {
            @display("p=576.0,232.0");
        };
        capsulate1: StressCapsulate {
            @display("p=255.0,60.0");
        };
        capsulate2: StressCapsulate {
            @display("p=404.0,287.0");
        };
        duplicate1: StressDuplicate {
            @display("p=444.0,100.0");
        };
        duplicate2: StressDuplicate {
            @display("p=215.0,247.0");
        };
        sink: StressSink {
            @display("p=66.0,117.0");
        };
    connections:
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
}
