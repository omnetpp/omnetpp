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


module ClosedQueueingNetA
    parameters:
        numTandems: numeric const, // number of switches (and also of tandem queues)
        numQueuesPerTandem: numeric const, // number of queues in a tandem queue
        sDelay: numeric,  // delay between switch and queues
        qDelay: numeric;  // delay between queues
    submodules:
        // switches
        switch: Switch[numTandems];
            gatesizes:
                out[numTandems];
            display: "p=60,60,m,1;i=block/switch";
        // queues
        queue: Queue[numTandems*numQueuesPerTandem];
            gatesizes if (index % numQueuesPerTandem)==0:
                in[numTandems];
            gatesizes if (index % numQueuesPerTandem)!=0:
                in[1];
            display: "p=150,90,m,$numQueuesPerTandem;i=block/activeq";
    connections:
        // connect switches to 1st queues
        for s=0..numTandems-1, q=0..numTandems-1 do
            switch[s].out[q] --> delay sDelay --> queue[q*numQueuesPerTandem].in[s];
        endfor;

        // connect queues to each other
        for s=0..numTandems-1, q=0..numQueuesPerTandem-2 do
            queue[s*numQueuesPerTandem+q].out --> delay qDelay --> queue[s*numQueuesPerTandem+q+1].in[0];
        endfor;

        // connect last queues back to switch
        for s=0..numTandems-1 do
            queue[(s+1)*numQueuesPerTandem-1].out --> delay qDelay --> switch[s].in;
        endfor;
endmodule


network cqnA : ClosedQueueingNetA
endnetwork


