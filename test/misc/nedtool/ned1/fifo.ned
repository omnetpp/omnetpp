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
    parameters:
        service_time: numeric,
        num_cells: numeric,
        num_init_jobs: numeric;
    gates:
        in: in;
        out: out;
endsimple
