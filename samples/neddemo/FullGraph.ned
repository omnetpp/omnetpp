//
// This file is part of an OMNeT++/OMNEST simulation example.
//
// Copyright (C) 1992-2005 Andras Varga
//
// This file is distributed WITHOUT ANY WARRANTY. See the file
// `license' for details on this and other legal matters.
//


simple FullGraphNode extends Node
{
    gates:
        inout g[];
}


network FullGraph
{
    parameters:
        int n @prompt("Number of nodes") = default(10);
    submodules:
        node[n]: FullGraphNode {
            parameters:
                @display("p=,,ring");
            gates:
                g[n];
        }
    connections allowunconnected:
        for i=0..(n-2), for j=(i+1)..(n-1) {
            node[i].g[j] <--> node[j].g[i];
        }
}


