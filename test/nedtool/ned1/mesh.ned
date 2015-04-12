//
// This file is part of an OMNeT++/OMNEST simulation example.
//
// Copyright (C) 1992-2015 Andras Varga
//
// This file is distributed WITHOUT ANY WARRANTY. See the file
// `license' for details on this and other legal matters.
//


module Mesh
    parameters:
        height: numeric const,
        width: numeric const;
    submodules:
        node: Node[height*width];
            parameters:
                address = index;
            gatesizes:
                in[4],
                out[4];
            display: "p=,,m,$width,40,40;i=misc/node_vs";
    connections nocheck:
        for i=0..height-1, j=0..width-1 do
            node[i*width+j].out[0] --> node[(i+1)*width+j].in[1] if i!=height-1;
            node[i*width+j].in[0] <-- node[(i+1)*width+j].out[1] if i!=height-1;
            node[i*width+j].out[2] --> node[i*width+j+1].in[3] if j!=width-1;
            node[i*width+j].in[2] <-- node[i*width+j+1].out[3] if j!=width-1;
        endfor;
endmodule


network mesh : Mesh
    parameters:
        height = input(9,"Number of rows"),
        width = input(7,"Number of columns");
endnetwork

