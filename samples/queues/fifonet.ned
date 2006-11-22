//
// This file is part of an OMNeT++/OMNEST simulation example.
//
// Contributed by Nick van Foreest
//
// This file is distributed WITHOUT ANY WARRANTY. See the file
// `license' for details on this and other legal matters.
//


import "gen";
import "fifo";
import "sink";

//
// Generator + FIFO + sink
//
module FifoNet
{
    parameters:
        int num_cells;
        int num_buffers;
    submodules:
        gen: Generator {
            parameters:
                num_messages;
                ia_time;
                @display("p=35,152;i=block/source");
        };
        fifo[num_buffers]: Fifo {
            parameters:
                num_cells = num_cells;
                service_time;
                num_init_jobs;
                @display("p=125,152,m;i=block/queue;q=queue");
        };
        sink: Sink {
            parameters:
                @display("p=;i=block/sink");
        };
    connections:
        gen.out --> fifo[0].in;
        for i=0..num_buffers-2 {
            fifo[i].out --> fifo[i+1].in;
        };
        fifo[num_buffers-1].out --> sink.in;
}

network fifonet extends FifoNet
{
    parameters:
        num_cells = default(20) @prompt("Number of cells: ");
}

