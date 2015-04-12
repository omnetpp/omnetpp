//
// This file is part of an OMNeT++/OMNEST simulation example.
//
// Copyright (C) 1992-2015 Andras Varga
//
// This file is distributed WITHOUT ANY WARRANTY. See the file
// `license' for details on this and other legal matters.
//


simple GridNode extends Node
{
    gates:
        inout up;
        inout left;
        inout down;
        inout right;
}


network Grid
{
    parameters:
        int height @prompt("Number of rows") = default(4);
        int width @prompt("Number of columns") = default(6);
    submodules:
        node[height*width]: GridNode {
            parameters:
                @display("p=,,m,$width");
        }
    connections allowunconnected:
        for i=0..height-1, for j=0..width-1 {
            node[i*width+j].down <--> node[(i+1)*width+j].up if i!=height-1;
            node[i*width+j].right <--> node[(i*width+j)+1].left if j!=width-1;
        }
}


