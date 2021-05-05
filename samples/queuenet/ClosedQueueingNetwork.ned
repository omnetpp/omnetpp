//
// This file is part of an OMNeT++/OMNEST simulation example.
//
// Copyright (C) 1992-2015 Andras Varga
//
// This file is distributed WITHOUT ANY WARRANTY. See the file
// `license' for details on this and other legal matters.
//

import org.omnetpp.queueing.Router;
import org.omnetpp.queueing.Queue;
import org.omnetpp.queueing.SourceOnce;

//
// A Closed Queueing Network, consisting of several tandem queues
//
network ClosedQueueingNetwork
{
    parameters:
        int numTandems;
        double sDelay @unit(s);
    types:
        module TandemQueue
        {
            parameters:
                int numQueues;
                int ownIndex;
                double sDelay @unit(s);
                double qDelay @unit(s);
                @display("i=misc/cloud2_l");
            gates:
                input in[];
                output out[]; // in[] and out[] sizes must match

            submodules:
                switch: Router {
                    parameters:
                        routingAlgorithm = "random";
                        @display("p=60,80");
                    gates:
                        out[sizeof(parent.in)];
                }
                queue[numQueues]: Queue {
                    parameters:
                        @display("p=60,160,row,80");
                    gates:
                        in[(index==0 ? sizeof(parent.in)+1 : 1)];
                }
                initialSource: SourceOnce {
                    @display("p=150,40");
                }
            connections allowunconnected:
                // connect switch outputs outside
                for i=0..sizeof(in)-1 {
                    switch.out[i] --> out[i] if i!=ownIndex;
                }

                // connect inputs to first queue inputs
                for i=0..sizeof(in)-1 {
                    in[i] --> queue[0].in[i] if i!=ownIndex;
                }

                // connect switch to first queue
                switch.out[ownIndex] --> {  delay = parent.sDelay; } --> queue[0].in[ownIndex];

                // connect queues to each other
                for i=0..numQueues-2 {
                    queue[i].out --> {  delay = parent.qDelay; } --> queue[i+1].in[0];
                }

                // connect last queue back to switch
                queue[numQueues-1].out --> {  delay = parent.qDelay; } --> switch.in++;

                // inject initial jobs into the first queue
                initialSource.out --> queue[0].in[sizeof(in)];
        }


    submodules:
        tandemQueue[numTandems]: TandemQueue {
            parameters:
                sDelay = parent.sDelay;
                ownIndex = index;
            gates:
                in[parent.numTandems];
                out[parent.numTandems];
        }
    connections allowunconnected:
        for i=0..numTandems-1, for j=0..numTandems-1 {
            tandemQueue[i].out[j] --> {  delay = parent.sDelay; } --> tandemQueue[j].in[i] if i!=j;
        }
}


