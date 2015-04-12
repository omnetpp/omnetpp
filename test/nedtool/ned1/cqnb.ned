//
// This file is part of an OMNeT++/OMNEST simulation example.
//
// Copyright (C) 1992-2015 Andras Varga
//
// This file is distributed WITHOUT ANY WARRANTY. See the file
// `license' for details on this and other legal matters.
//


import "tandemqueue.ned";

module ClosedQueueingNetB
    parameters:
        numTandems: numeric const,
        sDelay: numeric;
    submodules:
        tandemQueue: TandemQueue[numTandems];
            parameters:
                sDelay = sDelay,
                ownIndex = index;
            gatesizes:
                in[numTandems],
                out[numTandems];
            display: "i=misc/cloud2";
    connections nocheck:
        for i=0..numTandems-1, j=0..numTandems-1 do
            tandemQueue[i].out[j] --> delay sDelay --> tandemQueue[j].in[i] if i!=j;
        endfor;
endmodule

network cqnB : ClosedQueueingNetB
endnetwork

