// -------------------------------------------------------------
// file: closednet.ned
//        (part of Queues - an OMNeT++ demo simulation)
// -------------------------------------------------------------


import
    "fifo";

// This is a closed ring network --
//
// generator + FIFO + sink
//
module Ring
    parameters:
        num_cells : numeric,
        num_buffers : numeric;
    submodules:
        fifo: Fifo[num_buffers];
            parameters:
                num_cells = num_cells,
                service_time = input,
                num_init_jobs = input;
            display: "p=125,152;i=queue";
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

