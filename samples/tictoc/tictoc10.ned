//
// This file is part of an OMNeT++/OMNEST simulation example.
//
// Copyright (C) 2003-2008 Andras Varga
//
// This file is distributed WITHOUT ANY WARRANTY. See the file
// `license' for details on this and other legal matters.
//


simple Txc10
{
    parameters:
        @display("i=block/routing");
    gates:
        inout gate[];  // declare two way connections
}

//
// same as the previous step 
//  
network Tictoc10
{
    types:
        channel chn extends ned.DelayChannel {
            delay = 100ms;
        }
    submodules:
        tic[6]: Txc10;
    connections:
        tic[0].gate++ <--> chn <--> tic[1].gate++;
        tic[1].gate++ <--> chn <--> tic[2].gate++;
        tic[1].gate++ <--> chn <--> tic[4].gate++;
        tic[3].gate++ <--> chn <--> tic[4].gate++;
        tic[4].gate++ <--> chn <--> tic[5].gate++;
}

