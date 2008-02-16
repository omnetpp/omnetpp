//
// This file is part of an OMNeT++/OMNEST simulation example.
//
// Copyright (C) 1992-2005 Andras Varga
//
// This file is distributed WITHOUT ANY WARRANTY. See the file
// `license' for details on this and other legal matters.
//

package networks;

import node.Node;

network FullGraph
{
    parameters:
        double n = default(15) @prompt("Number of nodes");
    submodules:
        node[n]: Node {
            parameters:
                address = index;
                @display("p=,,ring");
            gates:
                port[n];
        };
    connections:
        for i=0..(n-2), for j=(i+1)..(n-1) {
            node[i].port[j] <--> node[j].port[i];
        };
}


