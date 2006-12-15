//
// This file is part of an OMNeT++/OMNEST simulation example.
//
// Copyright (C) 1992-2005 Andras Varga
//
// This file is distributed WITHOUT ANY WARRANTY. See the file
// `license' for details on this and other legal matters.
//



simple RndNode
{
    parameters:
        @display("i=misc/node_vs");
    gates:
        input in[];
        output out[];
}


network RandomGraph
{
    parameters:
        int n;
        volatile double connectedness; // 0.0<x<1.0
    submodules:
        node[n]: RndNode {};
    connections allowunconnected:
        for i=0..n-1, for j=0..n-1 {
            node[i].out++ --> node[j].in++ if i!=j && uniform(0, 1)<connectedness;
        };
}


