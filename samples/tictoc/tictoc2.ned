//
// This file is part of an OMNeT++/OMNEST simulation example.
//
// Copyright (C) 2003 Ahmet Sekercioglu
// Copyright (C) 2003-2005 Andras Varga
//
// This file is distributed WITHOUT ANY WARRANTY. See the file
// `license' for details on this and other legal matters.
//


simple Txc2
{
    parameters:
        @display("i=block/process");
    gates:
        input in;
        output out;
}

//
// Here we make the model look a bit prettier in the GUI. We assign
// the "proc1" icon (bitmaps/proc1.gif), and paint it cyan for `tic'
// and yellow for `toc'.
//
module Tictoc2
{
    submodules:
        tic: Txc2 {
            parameters:
                @display("i=,cyan"); // Note "i=<icon>,<color>"
        };
        toc: Txc2 {
            parameters:
                @display("i=,gold"); // Here too
        };
    connections:
        tic.out --> {  delay = 100ms; } --> toc.in;
        tic.in <-- {  delay = 100ms; } <-- toc.out;
}

network tictoc2 extends Tictoc2
{
}

