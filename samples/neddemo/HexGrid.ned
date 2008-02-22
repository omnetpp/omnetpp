//
// This file is part of an OMNeT++/OMNEST simulation example.
//
// Copyright (C) 1992-2005 Andras Varga
//
// This file is distributed WITHOUT ANY WARRANTY. See the file
// `license' for details on this and other legal matters.
//


simple HexGridNode extends Node
{
    gates:
        inout n;
        inout nw;
        inout sw;
        inout s;
        inout se;
        inout ne;
}


network HexGrid
{
    parameters:
        int rows @prompt("Number of rows") = default(7);
        int cols @prompt("Number of columns") = default(3);
    submodules:
        node[rows*cols]: HexGridNode;
    connections allowunconnected:
        for i=0..rows*cols-1 {
            node[i].n <--> node[i-2*cols].s if i+1>2*cols;
            node[i].nw <--> node[(i-cols)-(i/cols+1)%2].se if i%(2*cols)!=0 && i+1>cols;
            node[i].sw <--> node[((i+cols)-1)+(i/cols)%2].ne if i%(2*cols)!=0 && i<rows*cols-cols;
        }
}


