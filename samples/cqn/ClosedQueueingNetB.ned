//
// This file is part of an OMNeT++/OMNEST simulation example.
//
// Copyright (C) 1992-2015 Andras Varga
//
// This file is distributed WITHOUT ANY WARRANTY. See the file
// `license' for details on this and other legal matters.
//

//
// Closed Queueing Network, variant "B".
//
network ClosedQueueingNetB
{
    parameters:
        int numTandems;
        double sDelay @unit(s);
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


