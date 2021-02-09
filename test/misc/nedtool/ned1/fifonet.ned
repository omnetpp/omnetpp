//
// This file is part of an OMNeT++/OMNEST simulation example.
//
// Contributed by Nick van Foreest
//
// This file is distributed WITHOUT ANY WARRANTY. See the file
// `license' for details on this and other legal matters.
//


import
    "gen",
    "fifo",
    "sink";

//
// Generator + FIFO + sink
//
module FifoNet
    parameters:
        num_cells: numeric,
        num_buffers: numeric;
    submodules:
        gen: Generator;
            parameters:
                num_messages = input,
                ia_time = input;
            display: "p=35,152;i=block/source";
        fifo: Fifo[num_buffers];
            parameters:
                num_cells = num_cells,
                service_time = input,
                num_init_jobs = input;
            display: "p=125,152,m;i=block/activeq";
        sink: Sink;
            display: "p=;i=block/sink";
    connections:
        gen.out --> fifo[0].in;
        for i=0..num_buffers-2 do
            fifo[i].out --> fifo[i+1].in;
        endfor;
        fifo[num_buffers-1].out --> sink.in;
endmodule

network fifonet : FifoNet
    parameters:
        num_cells = input( 20, "Number of cells: ");
endnetwork

