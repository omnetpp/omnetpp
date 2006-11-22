//
// This file is part of an OMNeT++/OMNEST simulation example.
//
// Copyright (C) 1992-2005 Andras Varga
//
// This file is distributed WITHOUT ANY WARRANTY. See the file
// `license' for details on this and other legal matters.
//


simple FullGraphNode
{
    gates:
        input in[];
        output out[];
}


module FullGraph
{
    parameters:
        double n;
    submodules:
        node[n]: FullGraphNode {
            parameters:
                @display("i=misc/node_vs;p=,,ring");
            gates:
                in[n];
                out[n];
        };
    connections allowunconnected:
        for i=0..n-1, for j=0..n-1 {
            node[i].out[j] --> node[j].in[i] if i!=j;
        };
}


network fullGraph extends FullGraph
{
    parameters:
        n = default(10) @prompt("Number of nodes");
}

