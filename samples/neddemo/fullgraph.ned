//
// This file is part of an OMNeT++ simulation example.
//
// Copyright (C) 1992-2003 Andras Varga
//
// This file is distributed WITHOUT ANY WARRANTY. See the file
// `license' for details on this and other legal matters.
//


simple FullGraphNode
    gates:
        in: in[];
        out: out[];
endsimple


module FullGraph
    parameters:
        n : numeric const;
    submodules:
        node: FullGraphNode[n];
            gatesizes:
                in[n],
                out[n];
            display: "i=ball2_s;p=,,ring";
    connections nocheck:
        for i=0..n-1, j=0..n-1 do
            node[i].out[j] --> node[j].in[i] if i!=j;
        endfor;
endmodule


network fullGraph : FullGraph
    parameters:
        n = input(10,"Number of nodes");
endnetwork

