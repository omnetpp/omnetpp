// -------------------------------------------------------------
// file: fifonet.ned
//        (part of Queues - an OMNeT++ demo simulation)
// -------------------------------------------------------------


import
    "gen",
    "fifo",
    "sink";

// FifoNet --
//
// generator + FIFO + sink
//
module FifoNet
    parameters:
        num_cells : numeric,
        num_buffers : numeric;
    submodules:
        gen: Generator;
            parameters:
                num_messages = input,
                ia_time = input;
            display: "p=35,152;i=gen";
        fifo: Fifo[num_buffers];
            parameters:
                num_cells = num_cells,
                service_time = input,
                num_init_jobs = input;
            display: "p=125,152;i=queue";
        sink: Sink;
            display: "p=;i=sink";
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

