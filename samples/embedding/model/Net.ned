//
// This file is part of an OMNeT++/OMNEST simulation example.
//
// Copyright (C) 1992-2015 Andras Varga
//
// This file is distributed WITHOUT ANY WARRANTY. See the file
// `license' for details on this and other legal matters.
//

//
// Source+Sink example
//
network Net
{
    submodules:
        gen: Source;
        sink: Sink;
        fifo: Fifo {
            @display("p=114,156");
            serviceTime = exponential(1s);
        }
    connections:
        gen.out --> fifo.in;
        fifo.out --> sink.in;
}


