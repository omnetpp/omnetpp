//
// This file is part of an OMNeT++/OMNEST simulation example.
//
// Copyright (C) 2003-2005 Andras Varga
//
// This file is distributed WITHOUT ANY WARRANTY. See the file
// `license' for details on this and other legal matters.
//


simple Txc6
{
    parameters:
        volatile double delayTime;   // delay before sending back message
    gates:
        input in;
        output out;
}

module Tictoc6
{
    submodules:
        tic: Txc6 {
            parameters:
                @display("i=block/process,cyan");
        };
        toc: Txc6 {
            parameters:
                @display("i=block/process,gold");
        };
    connections:
        tic.out --> {  delay = 100ms; } --> toc.in;
        tic.in <-- {  delay = 100ms; } <-- toc.out;
}

network tictoc6 extends Tictoc6
{
}

