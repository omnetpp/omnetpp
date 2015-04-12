//
// This file is part of an OMNeT++/OMNEST simulation example.
//
// Copyright (C) 1992-2015 Andras Varga
//
// This file is distributed WITHOUT ANY WARRANTY. See the file
// `license' for details on this and other legal matters.
//

simple TriGridNode extends Node
{
    gates:
        inout w;
        inout nw;
        inout sw;
        inout e;
        inout se;
        inout ne;
}

network TriangleGrid
{
    parameters:
        int rows @prompt("Number of rows") = default(3);
        int cols @prompt("Number of columns") = default(7);
    submodules:
        node[rows*cols]: TriGridNode;
    connections allowunconnected:
        for x=0..cols-1, for y=0..rows-1 {
            node[y*cols+x].e <--> node[y*cols+x+1].w if x<cols-1;
        node[y*cols+x].se <--> node[(y+1)*cols+x].nw if y<rows-1;
        node[y*cols+x].sw <--> node[(y+1)*cols+x-1].ne if x>0 && y<rows-1;
        }
}
