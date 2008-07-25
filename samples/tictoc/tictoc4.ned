//
// This file is part of an OMNeT++/OMNEST simulation example.
//
// Copyright (C) 2003-2008 Andras Varga
//
// This file is distributed WITHOUT ANY WARRANTY. See the file
// `license' for details on this and other legal matters.
//


simple Txc4
{
    parameters:
        int limit;  // declare the new parameter
        @display("i=block/process");
    gates:
        input in;
        output out;
}

//
// Adding module parameters.
//
network Tictoc4
{
    submodules:
        tic: Txc4 {
            parameters:
                limit = 8;  // tic's limit is 8
                @display("i=,cyan");
        }
        toc: Txc4 {
            parameters:
                // note that we leave toc's limit unbound here
                @display("i=,gold");
        }
    connections:
        tic.out --> {  delay = 100ms; } --> toc.in;
        tic.in <-- {  delay = 100ms; } <-- toc.out;
}

