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
    gates:
        input in[];
        output out[];
}


module RandomGraph
{
    parameters:
        int n;
        volatile double connectedness; // 0.0<x<1.0
    submodules:
        node[n]: RndNode {
            parameters:
                @display("i=misc/node_vs");
        };
    connections allowunconnected:
        for i=0..n-1, for j=0..n-1 {
            node[i].out++ --> node[j].in++ if i!=j && uniform(0, 1)<connectedness;
        };
}


network randomGraph extends RandomGraph
{
    parameters:
        n = 10;
        connectedness = 0.3;
}

