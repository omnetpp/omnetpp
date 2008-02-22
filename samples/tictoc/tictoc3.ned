//
// This file is part of an OMNeT++/OMNEST simulation example.
//
// Copyright (C) 2003-2005 Andras Varga
//
// This file is distributed WITHOUT ANY WARRANTY. See the file
// `license' for details on this and other legal matters.
//


simple Txc3
{
    parameters:
        @display("i=block/process");  // make block/process the default icon
    gates:
        input in;
        output out;
}

//
// Same as Tictoc2.
//
network Tictoc3
{
    submodules:
        tic: Txc3 {
            parameters:
                @display("i=,cyan");  // colorize the default icon
        }
        toc: Txc3 {
            parameters:
                @display("i=,gold");  // here too
        }
    connections:
        tic.out --> {  delay = 100ms; } --> toc.in;
        tic.in <-- {  delay = 100ms; } <-- toc.out;
}

