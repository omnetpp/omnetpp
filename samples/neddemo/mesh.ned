//
// This file is part of an OMNeT++/OMNEST simulation example.
//
// Copyright (C) 1992-2005 Andras Varga
//
// This file is distributed WITHOUT ANY WARRANTY. See the file
// `license' for details on this and other legal matters.
//


simple MeshNode
    gates:
        in: up, left;
        out: down, right;
endsimple


module Mesh
    parameters:
        height : numeric const,
        width : numeric const;
    submodules:
        node: MeshNode[height*width];
            display: "p=,,m,$width;i=misc/node_s";
    connections nocheck:
        for i=0..height-1, j=0..width-1 do
            node[i*width+j].down --> node[(i+1)*width+j].up if i!=height-1;
            node[i*width+j].right --> node[i*width+j+1].left if j!=width-1;
        endfor;
endmodule


network mesh : Mesh
    parameters:
        height = input(4,"Number of rows"),
        width = input(6,"Number of columns");
endnetwork

