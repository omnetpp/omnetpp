//
// This file is part of an OMNeT++/OMNEST simulation example.
//
// Copyright (C) 2003-2008 Andras Varga
//
// This file is distributed WITHOUT ANY WARRANTY. See the file
// `license' for details on this and other legal matters.
//


simple Txc9c
{
    parameters:
        @display("i=block/routing");
    gates:
        inout gate[];  // declare two way connections
}

// using two way connections to further simplify the network definition 
network Tictoc9c
{
    types:
        channel chn extends ned.DelayChannel {
            delay = 100ms;
        }
    submodules:
        tic[6]: Txc9c;
    connections:
        tic[0].gate++ <--> chn <--> tic[1].gate++;
        tic[1].gate++ <--> chn <--> tic[2].gate++;
        tic[1].gate++ <--> chn <--> tic[4].gate++;
        tic[3].gate++ <--> chn <--> tic[4].gate++;
        tic[4].gate++ <--> chn <--> tic[5].gate++;
}

