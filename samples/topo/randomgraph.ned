//
// This file is part of an OMNeT++ simulation example.
//
// Copyright (C) 1992-2003 Andras Varga
//
// This file is distributed WITHOUT ANY WARRANTY. See the file
// `license' for details on this and other legal matters.
//



simple RndNode
    parameters:
        x,y;
    gates:
        in: in[];
        out: out[];
endsimple


module RandomGraph
    parameters:
        n : numeric const,
        connectedness : numeric; // 0.0<x<1.0
    submodules:
        node: RndNode[n];
            parameters:
                x = uniform(0,350),
                y = uniform(0,350);
            gatesizes:
                in[n],
                out[n];
            display: "p=20,20,exact,$x,$y;i=ball2_s";
    connections nocheck:
        for i=0..n-1, j=0..n-1 do
            node[i].out[j] --> node[j].in[i] if i!=j && uniform(0,1) < connectedness;
        endfor;
endmodule


network randomGraph : RandomGraph
    parameters:
        n = 10,
        connectedness = 0.3;
endnetwork

