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
    parameters:
        num_cells: numeric,
        num_buffers: numeric;
    submodules:
        fifo: Fifo[num_buffers];
            parameters:
                num_cells = num_cells,
                service_time = input,
                num_init_jobs = input;
            display: "i=block/activeq";
    connections:
        for i=0..num_buffers-2 do
            fifo[i].out --> fifo[i+1].in;
        endfor;
        fifo[num_buffers-1].out --> fifo[0].in;
endmodule

network ring : Ring
    parameters:
        num_cells = input( 20, "Number of cells: ");
endnetwork

