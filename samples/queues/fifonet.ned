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
        int numCells;
        int numBuffers;
    submodules:
        gen: Generator {
            parameters:
                numMessages;
                iaTime;
                @display("p=35,152;i=block/source");
        };
        fifo[numBuffers]: Fifo {
            parameters:
                numCells = numCells;
                serviceTime;
                numInitJobs;
                @display("p=125,152,m;i=block/queue;q=queue");
        };
        sink: Sink {
            parameters:
                @display("p=;i=block/sink");
        };
    connections:
        gen.out --> fifo[0].in;
        for i=0..numBuffers-2 {
            fifo[i].out --> fifo[i+1].in;
        };
        fifo[numBuffers-1].out --> sink.in;
}

network fifonet extends FifoNet
{
    parameters:
        numCells = default(20) @prompt("Number of cells: ");
}

