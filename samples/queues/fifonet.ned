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
                @display("p=35,152");
        };
        fifo[numBuffers]: Fifo {
            parameters:
                numCells = numCells;
                @display("p=125,152,m;q=queue");
        };
        sink: Sink {
            parameters:
                @display("p=");
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

