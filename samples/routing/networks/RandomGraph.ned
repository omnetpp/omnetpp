//
// This file is part of an OMNeT++/OMNEST simulation example.
//
// Copyright (C) 1992-2015 Andras Varga
//
// This file is distributed WITHOUT ANY WARRANTY. See the file
// `license' for details on this and other legal matters.
//

package networks;

import node.Node;

network RandomGraph
{
    parameters:
        int n @prompt("Number of nodes") = default(15);
    submodules:
        node[n]: Node {
            parameters:
                address = index;
                @display("p=100,100,ring");
            gates:
                port[parent.n];
        }
    connections allowunconnected:
        for i=0..n-2, for j=i+1..n-1 {
            node[i].port[j] <--> {  delay = 0.1ms; datarate = 1Gbps; } <--> node[j].port[i] if uniform(0,1)<0.15;
        }
}


