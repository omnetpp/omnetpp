//
// This file is part of an OMNeT++/OMNEST simulation example.
//
// Contributed by Nick van Foreest
//
// This file is distributed WITHOUT ANY WARRANTY. See the file
// `license' for details on this and other legal matters.
//


//
// Single-server queue, same service rate for each packet
//
simple Fifo
{
    parameters:
        int numCells;
        int numInitJobs;
        volatile double serviceTime;
        @display("i=block/queue;q=queue");
    gates:
        input in;
        output out;
}
