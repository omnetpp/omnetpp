//
// This file is part of an OMNeT++/OMNEST simulation example.
//
// Copyright (C) 1992-2015 Andras Varga
//
// This file is distributed WITHOUT ANY WARRANTY. See the file
// `license' for details on this and other legal matters.
//

import "../node";

module FullGraph
    parameters:
        n: numeric const;
    submodules:
        node: Node[n];
            parameters:
                address = index;
            gatesizes:
                in[n],
                out[n];
            display: "i=misc/node_vs;p=,,ring";
    connections nocheck:
        for i=0..n-1, j=0..n-1 do
            node[i].out[j] --> node[j].in[i] if i!=j;
        endfor;
endmodule


network fullGraph : FullGraph
    parameters:
        n = input(15,"Number of nodes");
endnetwork

