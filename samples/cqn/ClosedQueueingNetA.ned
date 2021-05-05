//
// This file is part of an OMNeT++/OMNEST simulation example.
//
// Copyright (C) 1992-2015 Andras Varga
//
// This file is distributed WITHOUT ANY WARRANTY. See the file
// `license' for details on this and other legal matters.
//

//
// Closed Queueing Network, variant "A".
//
network ClosedQueueingNetA
{
    parameters:
        int numTandems; // number of switches (and also of tandem queues)
        int numQueuesPerTandem; // number of queues in a tandem queue
        double sDelay @unit(s);  // delay between switch and queues
        double qDelay @unit(s);  // delay between queues
    submodules:
        // switches
        switch[numTandems]: Switch {
            parameters:
                @display("p=60,60,m,1");
            gates:
                out[parent.numTandems];
        }
        // queues
        queue[numTandems*numQueuesPerTandem]: Queue {
            parameters:
                @display("p=150,90,m,$numQueuesPerTandem");
            gates:
                in[(index%parent.numQueuesPerTandem==0 ? parent.numTandems : 1)];
        }
    connections:
        // connect switches to 1st queues
        for s=0..numTandems-1, for q=0..numTandems-1 {
            switch[s].out[q] --> { delay = parent.sDelay; } --> queue[q*numQueuesPerTandem].in[s];
        }

        // connect queues to each other
        for s=0..numTandems-1, for q=0..numQueuesPerTandem-2 {
            queue[s*numQueuesPerTandem+q].out --> { delay = parent.qDelay; } --> queue[(s*numQueuesPerTandem+q)+1].in[0];
        }

        // connect last queues back to switch
        for s=0..numTandems-1 {
            queue[(s+1)*numQueuesPerTandem-1].out --> { delay = parent.qDelay; } --> switch[s].in;
        }
}


