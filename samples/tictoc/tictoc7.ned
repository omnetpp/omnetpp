//
// This file is part of an OMNeT++/OMNEST simulation example.
//
// Copyright (C) 2003-2005 Andras Varga
//
// This file is distributed WITHOUT ANY WARRANTY. See the file
// `license' for details on this and other legal matters.
//


simple Tic7
{
    gates:
        input in;
        output out;
}

simple Toc7
{
    gates:
        input in;
        output out;
}

module Tictoc7
{
    submodules:
        tic: Tic7 {
            parameters:
                @display("i=block/process,cyan");
        };
        toc: Toc7 {
            parameters:
                @display("i=block/process,gold");
        };
    connections:
        tic.out --> {  delay = 100ms; } --> toc.in;
        tic.in <-- {  delay = 100ms; } <-- toc.out;
}

network tictoc7 extends Tictoc7
{
}

