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
    parameters:
        @display("i=misc/node_s");
    gates:
        input left;
        output right;
}


module Chain
{
    parameters:
        int n = default(5) @prompt("Number of modules");
    submodules:
        node[n]: ChainNode;
    connections allowunconnected:
        for i=0..n-2 {
            node[i].right --> node[i+1].left;
        };
}


network chain extends Chain
{
}

