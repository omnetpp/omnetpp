//
// This file is part of an OMNeT++/OMNEST simulation example.
//
// Copyright (C) 1992-2005 Andras Varga
//
// This file is distributed WITHOUT ANY WARRANTY. See the file
// `license' for details on this and other legal matters.
//


simple StarNode
{
    gates:
        input in;
}


simple HubNode
{
    gates:
        output out[];
}


module Star
{
    parameters:
        int n;
    submodules:
        node[n]: StarNode {
            parameters:
                @display("p=50,50,ring,150,150;i=device/pc2_s");
        };
        hub: HubNode {
            parameters:
                @display("p=200,200;i=device/switch");
            gates:
                out[n];
        };
    connections allowunconnected:
        for i=0..n-1 {
            hub.out[i] --> node[i].in;
        };
}


network star extends Star
{
    parameters:
        n = default(11) @prompt("Number of stations");
}

