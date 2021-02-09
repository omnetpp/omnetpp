//
// This file is part of an OMNeT++/OMNEST simulation example.
//
// Copyright (C) 1992-2015 Andras Varga
//
// This file is distributed WITHOUT ANY WARRANTY. See the file
// `license' for details on this and other legal matters.
//


//
// Single-server queue. Configurable is the service time (which can also
// be random, i.e. different for each job), and the number jobs that
// are initially created and inserted into the queue.
//
simple Queue
{
    parameters:
        int numInitialJobs;
        volatile double serviceTime @unit(s);
        int priority = default(0);
        @display("i=block/activeq;q=queue");
    gates:
        input in[];
        output out;
}


