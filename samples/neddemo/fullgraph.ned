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
    parameters:
        @display("i=misc/node_vs");
    gates:
        input in[];
        output out[];
}


network FullGraph
{
    parameters:
        int n = default(10) @prompt("Number of nodes");
    submodules:
        node[n]: FullGraphNode {
            parameters:
                @display("p=,,ring");
            gates:
                in[n];
                out[n];
        };
    connections allowunconnected:
        for i=0..n-1, for j=0..n-1 {
            node[i].out[j] --> node[j].in[i] if i!=j;
        };
}


