//
// This file is part of an OMNeT++/OMNEST simulation example.
//
// Copyright (C) 1992-2015 Andras Varga
//
// This file is distributed WITHOUT ANY WARRANTY. See the file
// `license' for details on this and other legal matters.
//


simple StarNode extends Node
{
    parameters:
        @display("i=device/pc2_s");
    gates:
        inout g;
}


simple HubNode extends Node
{
    parameters:
        @display("i=device/switch");
    gates:
        inout g[];
}


network Star
{
    parameters:
        int n @prompt("Number of stations") = default(11);
    submodules:
        node[n]: StarNode {
            parameters:
                @display("p=50,50,ring,150,150");
        }
        hub: HubNode {
            parameters:
                @display("p=200,200");
            gates:
                g[parent.n];
        }
    connections allowunconnected:
        for i=0..n-1 {
            hub.g[i] <--> node[i].g;
        }
}


