//
// This file is part of an OMNeT++/OMNEST simulation example.
//
// Contributed by Nick van Foreest
//
// This file is distributed WITHOUT ANY WARRANTY. See the file
// `license' for details on this and other legal matters.
//


import "fifo";

//
// This is a closed ring network.
//
module Ring
{
    parameters:
        double num_cells;
        double num_buffers;
    submodules:
        fifo[num_buffers]: Fifo {
            parameters:
                num_cells = num_cells;
                service_time;
                num_init_jobs;
                @display("i=block/queue;q=queue");
        };
    connections:
        for i=0..num_buffers-2 {
            fifo[i].out --> fifo[i+1].in;
        };
        fifo[num_buffers-1].out --> fifo[0].in;
}

network ring extends Ring
{
    parameters:
        num_cells = default(20) @prompt("Number of cells: ");
}

