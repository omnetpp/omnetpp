//
// This file is part of an OMNeT++/OMNEST simulation example.
//
// Copyright (C) 2003-2015 Andras Varga
//
// This file is distributed WITHOUT ANY WARRANTY. See the file
// `license' for details on this and other legal matters.
//

simple Txc18 extends Txc16
{
}

network TicToc18
{
    parameters:
        int numCentralNodes = default(2);
    types:
        channel Channel extends ned.DelayChannel {
            delay = 100ms;
        }
    submodules:
        tic[numCentralNodes+4]: Txc18;
    connections:
        // connect the 2 nodes in one side to the central nodes
        tic[0].gate++ <--> Channel <--> tic[2].gate++;
        tic[1].gate++ <--> Channel <--> tic[2].gate++;
        // connect the central nodes together
        for i=2..numCentralNodes {
            tic[i].gate++ <--> Channel <--> tic[i+1].gate++;
        }
        // connect the 2 nodes on the other side to the central nodes
        tic[numCentralNodes+2].gate++ <--> Channel <--> tic[numCentralNodes+1].gate++;
        tic[numCentralNodes+3].gate++ <--> Channel <--> tic[numCentralNodes+1].gate++;
}
