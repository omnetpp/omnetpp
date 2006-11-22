//
// This file is part of an OMNeT++/OMNEST simulation example.
//
// Copyright (C) 1992-2005 Andras Varga
//
// This file is distributed WITHOUT ANY WARRANTY. See the file
// `license' for details on this and other legal matters.
//


simple HexGridNode
{
    gates:
        input n;
        input nw;
        input sw;
        output s;
        output se;
        output ne;
}


module HexGrid
{
    parameters:
        int rows;
        int cols;
    submodules:
        node[rows*cols]: HexGridNode {
            parameters:
                @display("i=misc/node_vs");
        };
    connections allowunconnected:
        for i=0..rows*cols-1 {
            node[i].n <-- node[i-2*cols].s if i+1>2*cols;
            node[i].nw <-- node[(i-cols)-(i/cols+1)%2].se if i%(2*cols)!=0 && i+1>cols;
            node[i].sw <-- node[((i+cols)-1)+(i/cols)%2].ne if i%(2*cols)!=0 && i<rows*cols-cols;
        };
}


network hexGrid extends HexGrid
{
    parameters:
        rows = default(7) @prompt("Number of rows");
        cols = default(3) @prompt("Number of columns");
}

