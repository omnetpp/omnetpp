//
// This file is part of an OMNeT++ simulation example.
//
// Copyright (C) 1992-2003 Andras Varga
//
// This file is distributed WITHOUT ANY WARRANTY. See the file
// `license' for details on this and other legal matters.
//


import
    "hc_rte.ned",
    "gensink.ned",
    "hcube.ned";


//
// Building block of the delection routing hypercube. The interface of
// this module conforms to the HCNode interface defined in hcube.ned.
//
module HypercubeNode
    parameters:
        address : numeric,
        dim : numeric;
    gates:
        in: in[];
        out: out[];
    submodules:
        gen: HCGenerator;
            parameters:
                num_stations = 2^dim,
                address = address,
                ia_time = input;
        sink: HCSink;
            parameters:
                address = address;
        rte: HCRouter;
            parameters:
                address = address,
                dim = dim;
            gatesizes:
                in[dim],
                out[dim];
    connections:
        gen.out --> rte.from_gen;
        sink.in <-- rte.to_sink;
        for i=0..dim-1 do
            in[i] --> rte.in[i];
            out[i] <-- rte.out[i];
        endfor;
endmodule


//
// The actual hypercube network, instantiated as from the Hypercube
// topology template, substituting HypercubeNode for HCNode.
//
network hypercube : Hypercube
    parameters:
        dim = input(2,"Dimension (1,2,3,4,...)"),
        nodetype = "HypercubeNode";
endnetwork


