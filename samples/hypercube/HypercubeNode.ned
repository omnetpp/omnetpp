//
// This file is part of an OMNeT++/OMNEST simulation example.
//
// Copyright (C) 1992-2015 Andras Varga
//
// This file is distributed WITHOUT ANY WARRANTY. See the file
// `license' for details on this and other legal matters.
//

//
// Building block of the deflection routing hypercube.
//
module HypercubeNode like IHypercubeNode
{
    parameters:
        int address;
        int dim;
        @display("i=block/circle;is=vs");
    gates:
        inout port[dim];
    submodules:
        gen: HCGenerator {
            parameters:
                numStations = 2^parent.dim;
                address = parent.address;
                iaTime = default(exponential(1s));
                @display("p=82,82");
        }
        sink: HCSink {
            parameters:
                address = parent.address;
                @display("p=176,82");
        }
        rte: HCRouter {
            parameters:
                address = parent.address;
                dim = parent.dim;
                @display("p=125,171");
            gates:
                in[dim];
                out[dim];
        }
    connections:
        gen.out --> rte.fromGen;
        sink.in <-- rte.toSink;
        for i=0..dim-1 {
            port$i[i] --> rte.in[i];
            port$o[i] <-- rte.out[i];
        }
}


