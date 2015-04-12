//
// This file is part of an OMNeT++/OMNEST simulation example.
//
// Copyright (C) 1992-2015 Andras Varga
//
// This file is distributed WITHOUT ANY WARRANTY. See the file
// `license' for details on this and other legal matters.
//


simple HexGridNode
    gates:
        in:  n,nw,sw;
        out: s,se,ne;
endsimple


module HexGrid
    parameters:
        rows: numeric,
        cols: numeric;
    submodules:
        node: HexGridNode[ rows * cols ];
            display: "i=misc/node_vs";
    connections nocheck:
        for i = 0 .. rows*cols-1 do
            node[i].n <-- node[i-2*cols].s if i+1>2*cols;
            node[i].nw <-- node[i-cols-(i/cols+1)%2].se if i%(2*cols)!=0 && i+1>cols;
            node[i].sw <-- node[i+cols-1+(i/cols)%2].ne if i%(2*cols)!=0 && i<rows*cols-cols;
        endfor;
endmodule


network hexGrid : HexGrid
    parameters:
        rows = input(7,"Number of rows"),
        cols = input(3,"Number of columns");
endnetwork

