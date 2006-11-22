//
// This file is part of an OMNeT++/OMNEST simulation example.
//
// Copyright (C) 1992-2005 Andras Varga
//
// This file is distributed WITHOUT ANY WARRANTY. See the file
// `license' for details on this and other legal matters.
//



simple ChainNode
{
    gates:
        input left;
        output right;
}


module Chain
{
    parameters:
        int n;
    submodules:
        node[n]: ChainNode {
            parameters:
                @display("i=misc/node_s");
        };
    connections allowunconnected:
        for i=0..n-2 {
            node[i].right --> node[i+1].left;
        };
}


network chain extends Chain
{
    parameters:
        n = default(5) @prompt("Number of modules");
}

