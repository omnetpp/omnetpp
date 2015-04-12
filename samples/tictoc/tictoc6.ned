//
// This file is part of an OMNeT++/OMNEST simulation example.
//
// Copyright (C) 2003-2015 Andras Varga
//
// This file is distributed WITHOUT ANY WARRANTY. See the file
// `license' for details on this and other legal matters.
//


simple Txc6
{
    parameters:
        @display("i=block/routing");
    gates:
        input in;
        output out;
}

network Tictoc6
{
    submodules:
        tic: Txc6 {
            parameters:
                @display("i=,cyan");
        }
        toc: Txc6 {
            parameters:
                @display("i=,gold");
        }
    connections:
        tic.out --> {  delay = 100ms; } --> toc.in;
        tic.in <-- {  delay = 100ms; } <-- toc.out;
}

