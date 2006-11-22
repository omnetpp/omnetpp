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
        volatile double service_time;
        int num_cells;
        int num_init_jobs;
    gates:
        input in;
        output out;
}
