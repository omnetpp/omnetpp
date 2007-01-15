//
// This file is part of an OMNeT++/OMNEST simulation example.
//
// Copyright (C) 1992-2005 Andras Varga
//
// This file is distributed WITHOUT ANY WARRANTY. See the file
// `license' for details on this and other legal matters.
//


import "hc_rte.ned";
import "gensink.ned";
import "hcube.ned";


//
// Building block of the delection routing hypercube. The interface of
// this module conforms to the HCNode interface defined in hcube.ned.
//
module HypercubeNode like HCNode
{
    parameters:
        int address;
        int dim;
    gates:
        inout port[dim];
    submodules:
        gen: HCGenerator {
            parameters:
                numStations = 2^dim;
                address = address;
                iaTime = default(exponential(1));
                @display("p=82,82");
        };
        sink: HCSink {
            parameters:
                address = address;
                @display("p=176,82");
        };
        rte: HCRouter {
            parameters:
                address = address;
                dim = dim;
                @display("p=125,171");
            gates:
                in[dim];
                out[dim];
        };
    connections:
        gen.out --> rte.fromGen;
        sink.in <-- rte.toSink;
        for i=0..dim-1 {
            port$i[i] --> rte.in[i];
            port$o[i] <-- rte.out[i];
        };
}


//
// The actual hypercube network, instantiated as from the Hypercube
// topology template, substituting HypercubeNode for HCNode.
//
network hypercube extends Hypercube
{
    parameters:
        dim = default(2) @prompt("Dimension (1,2,3,4,...)");
        nodetype = "HypercubeNode";
}


