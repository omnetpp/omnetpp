//
// This file is part of an OMNeT++/OMNEST simulation example.
//
// Copyright (C) 1992-2015 Andras Varga
//
// This file is distributed WITHOUT ANY WARRANTY. See the file
// `license' for details on this and other legal matters.
//


import "queue.ned";
import "switch.ned";


module TandemQueue
    parameters:
        numQueues: numeric const,
        ownIndex: numeric const,
        sDelay: numeric,
        qDelay: numeric;
    gates:
        in: in[];
        out: out[]; // in[] and out[] sizes must match

    submodules:
        switch: Switch;
            gatesizes:
                out[sizeof(in)];
            display: "p=60,80;i=block/switch";

        queue: Queue[numQueues];
            gatesizes if index==0:
                in[sizeof(in)];
            gatesizes if index!=0:
                in[1];
            display: "p=60,160,row,80;i=block/activeq;q=queue";

    connections nocheck:
        // connect switch outputs outside
        for i=0..sizeof(in)-1 do
            switch.out[i] --> out[i] if i!=ownIndex;
        endfor;

        // connect inputs to first queue inputs
        for i=0..sizeof(in)-1 do
            in[i] --> queue[0].in[i] if i!=ownIndex;
        endfor;

        // connect switch to first queue
        switch.out[ownIndex] --> delay sDelay --> queue[0].in[ownIndex];

        // connect queues to each other
        for i=0..numQueues-2 do
            queue[i].out --> delay qDelay --> queue[i+1].in[0];
        endfor;

        // connect last queue back to switch
        queue[numQueues-1].out --> delay qDelay --> switch.in;

endmodule



