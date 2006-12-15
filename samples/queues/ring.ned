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
        int numCells;
        int numBuffers;
    submodules:
        fifo[numBuffers]: Fifo {
            parameters:
                numCells = numCells;
        };
    connections:
        for i=0..numBuffers-2 {
            fifo[i].out --> fifo[i+1].in;
        };
        fifo[numBuffers-1].out --> fifo[0].in;
}

network ring extends Ring
{
    parameters:
        numCells = default(20) @prompt("Number of cells: ");
}

