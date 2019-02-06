//
// This file is part of an OMNeT++/OMNEST simulation example.
//
// Copyright (C) 1992-2015 Andras Varga
//
// This file is distributed WITHOUT ANY WARRANTY. See the file
// `license' for details on this and other legal matters.
//


//
// Interface of modules types that can be used as a node in a Hypercube;
// see comment at the Hypercube module.
//
simple HCNode
    parameters:
        address: numeric,
        dim: numeric;
    gates:
        out: out[];
        in: in[];
endsimple


//
// Defines Hypercube topology.
//
// When the user creates an actual hypercube, he substitutes the name
// of an existing module type (e.g. HypercubeNode) for the nodetype
// parameter. The module type implements the algorithm the user wants
// to simulate and it must have the same gates that the HCNode type has.
//
// The topology template code can be used through file import.
// See hc_net.ned
//
module Hypercube
    parameters:
        dim: numeric const,
        nodetype: string;
    submodules:
        node: nodetype[2^dim] like HCNode;
            parameters:
                address = index, // module's position in (2^dim)-size vector
                dim = dim;
            gatesizes:
                out[dim],
                in[dim];
            display: "i=misc/node2_vs";
    connections:
        for i=0..2^dim-1, j=0..dim-1 do
            node[i].out[j] --> node[i # (1<<j)].in[j]; // # is bitwise xor
        endfor;
endmodule


