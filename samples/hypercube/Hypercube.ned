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
moduleinterface IHypercubeNode
{
    parameters:
        int address;
        int dim;
    gates:
        inout port[dim];
}


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
{
    parameters:
        int dim;
        string nodetype;
    submodules:
        node[2^dim]: <nodetype> like IHypercubeNode {
            parameters:
                address = index; // module's position in (2^dim)-size vector
                dim = parent.dim;
        }
    connections:
        for i=0..2^dim-1, for j=0..dim-1 {
            // connect node i to the node whose index differs only in the
            // jth bit (# stands for bitwise XOR); plus we need an "if" so that
            // we don't try to connect a gate from both directions.
            node[i].port[j] <--> node[i#(1<<j)].port[j] if (i < i#(1<<j));
        }
}


