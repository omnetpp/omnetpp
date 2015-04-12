//
// This file is part of an OMNeT++/OMNEST simulation example.
//
// Copyright (C) 1992-2015 Andras Varga
//
// This file is distributed WITHOUT ANY WARRANTY. See the file
// `license' for details on this and other legal matters.
//


simple StarNode
    gates:
        in: in;
endsimple


simple HubNode
    gates:
        out: out[];
endsimple


module Star
    parameters:
        n: numeric const;
    submodules:
        node: StarNode[n];
            display: "p=50,50,ring,150,150;i=device/pc2_s";
        hub: HubNode;
            gatesizes:
                out[n];
            display: "p=200,200;i=device/switch";
    connections nocheck:
        for i=0..n-1 do
            hub.out[i] --> node[i].in;
        endfor;
endmodule


network star : Star
    parameters:
        n = input(11,"Number of stations");
endnetwork

