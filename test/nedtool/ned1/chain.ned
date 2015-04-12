//
// This file is part of an OMNeT++/OMNEST simulation example.
//
// Copyright (C) 1992-2015 Andras Varga
//
// This file is distributed WITHOUT ANY WARRANTY. See the file
// `license' for details on this and other legal matters.
//



simple ChainNode
    gates:
        in: left;
        out: right;
endsimple


module Chain
    parameters:
        n: numeric const;
    submodules:
        node: ChainNode[n];
            display: "i=misc/node_s";
    connections nocheck:
        for i=0..n-2 do
            node[i].right --> node[i+1].left;
        endfor;
endmodule


network chain : Chain
    parameters:
        n = input(5,"Number of modules");
endnetwork

