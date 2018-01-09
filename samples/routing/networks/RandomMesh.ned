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

network RandomMesh
{
    parameters:
        int height @prompt("Number of rows") = default(8);
        int width @prompt("Number of columns") = default(8);
    types:
        channel C extends ned.DatarateChannel
        {
            parameters:
                delay = default(0.1ms);
                datarate = default(1Gbps);
        }
    submodules:
        node[height*width]: Node {
            parameters:
                address = index;
                @display("p=,,m,$width,40,40");
            gates:
                port[4];
        }
    connections allowunconnected:
        for i=0..height-1, for j=0..width-1 {
            node[i*width+j].port[0] <--> C <--> node[(i+1)*width+j].port[1] if i!=height-1 && uniform(0,1)<0.70;
            node[i*width+j].port[2] <--> C <--> node[(i*width+j)+1].port[3] if j!=width-1 && uniform(0,1)<0.70;
        }
}


