//
// This file is part of an OMNeT++/OMNEST simulation example.
//
// Copyright (C) 1992-2005 Andras Varga
//
// This file is distributed WITHOUT ANY WARRANTY. See the file
// `license' for details on this and other legal matters.
//


simple MeshNode
{
    gates:
        input up;
        input left;
        output down;
        output right;
}


module Mesh
{
    parameters:
        double height;
        double width;
    submodules:
        node[height*width]: MeshNode {
            parameters:
                @display("p=,,m,$width;i=misc/node_s");
        };
    connections allowunconnected:
        for i=0..height-1, for j=0..width-1 {
            node[i*width+j].down --> node[(i+1)*width+j].up if i!=height-1;
            node[i*width+j].right --> node[(i*width+j)+1].left if j!=width-1;
        };
}


network mesh extends Mesh
{
    parameters:
        height = default(4) @prompt("Number of rows");
        width = default(6) @prompt("Number of columns");
}

