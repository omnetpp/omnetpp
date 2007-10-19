//
// This file is part of an OMNeT++/OMNEST simulation example.
//
// Copyright (C) 1992-2005 Andras Varga
//
// This file is distributed WITHOUT ANY WARRANTY. See the file
// `license' for details on this and other legal matters.
//
package networks;

import Node;

network Mesh
{
    parameters:
        double height = default(9) @prompt("Number of rows");
        double width = default(7) @prompt("Number of columns");
    submodules:
        node[height*width]: Node {
            parameters:
                address = index;
                @display("p=,,m,$width,40,40");
            gates:
                port[4];
        };
    connections allowunconnected:
        //FIXME this has to be rewritten to use port[] instead of in[] / out[]
        for i=0..height-1, for j=0..width-1 {
            node[i*width+j].out[0] --> node[(i+1)*width+j].in[1] if i!=height-1;
            node[i*width+j].in[0] <-- node[(i+1)*width+j].out[1] if i!=height-1;
            node[i*width+j].out[2] --> node[(i*width+j)+1].in[3] if j!=width-1;
            node[i*width+j].in[2] <-- node[(i*width+j)+1].out[3] if j!=width-1;
        };
}


