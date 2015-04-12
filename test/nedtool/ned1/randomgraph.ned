//
// This file is part of an OMNeT++/OMNEST simulation example.
//
// Copyright (C) 1992-2015 Andras Varga
//
// This file is distributed WITHOUT ANY WARRANTY. See the file
// `license' for details on this and other legal matters.
//



simple RndNode
    gates:
        in: in[];
        out: out[];
endsimple


module RandomGraph
    parameters:
        n: numeric const,
        connectedness: numeric; // 0.0<x<1.0
    submodules:
        node: RndNode[n];
            display: "i=misc/node_vs";
    connections nocheck:
        for i=0..n-1, j=0..n-1 do
            node[i].out++ --> node[j].in++ if i!=j && uniform(0,1) < connectedness;
        endfor;
endmodule


network randomGraph : RandomGraph
    parameters:
        n = 10,
        connectedness = 0.3;
endnetwork

