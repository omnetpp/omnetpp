//
// This file is part of an OMNeT++/OMNEST simulation example.
//
// Copyright (C) 1992-2015 Andras Varga
//
// This file is distributed WITHOUT ANY WARRANTY. See the file
// `license' for details on this and other legal matters.
//


simple HexGridNode extends Node
{
    gates:
        inout port[];
}


network HexGrid
{
    parameters:
        int rows @prompt("HexMesh rows") = default(3);
        int cols @prompt("HexMesh columns") = default(3);
        int num = 2*(rows*cols+rows+cols);
    submodules:
        node[num]: HexGridNode;
    connections:
        for i=0..num-1 {
            node[i].port++ <--> node[i+1].port++ if i<num-1 && i%(2*cols+2)!=2*cols;
            node[i].port++ <--> node[i+2*cols+1].port++ if i<num-2*cols-1 && i%2==0;
        }
}
