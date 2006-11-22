//
// This file is part of an OMNeT++/OMNEST simulation example.
//
// Copyright (C) 1992-2005 Andras Varga
//
// This file is distributed WITHOUT ANY WARRANTY. See the file
// `license' for details on this and other legal matters.
//


import "tandemqueue.ned";


//
// Closed Queueing Network, variant "B".
//
module ClosedQueueingNetB
{
    parameters:
        int numTandems;
        double sDelay;
    submodules:
        tandemQueue[numTandems]: TandemQueue {
            parameters:
                sDelay = sDelay;
                ownIndex = index;
                @display("i=misc/cloud2_l");
            gates:
                in[numTandems];
                out[numTandems];
        };
    connections allowunconnected:
        for i=0..numTandems-1, for j=0..numTandems-1 {
            tandemQueue[i].out[j] --> {  delay = sDelay; } --> tandemQueue[j].in[i] if i!=j;
        };
}


//
// Instantiates ClosedQueueingNetB.
//
network cqnB extends ClosedQueueingNetB
{
}

