//
// This file is part of an OMNeT++/OMNEST simulation example.
//
// Copyright (C) 1992-2015 Andras Varga
//
// This file is distributed WITHOUT ANY WARRANTY. See the file
// `license' for details on this and other legal matters.
//


simple ChainNode extends Node
{
    parameters:
        @display("i=misc/square_vs");
    gates:
        inout left;
        inout right;
}


network Chain
{
    parameters:
        int n @prompt("Number of modules") = default(5);
    submodules:
        node[n]: ChainNode;
    connections allowunconnected:
        for i=0..n-2 {
            node[i].right <--> node[i+1].left;
        }
}


