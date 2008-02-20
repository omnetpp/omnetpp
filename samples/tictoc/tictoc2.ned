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
    gates:
        input in;
        output out;
}

//
// Here we make the model look a bit prettier in the GUI. We assign the
// "block/process" icon, and paint it cyan for `tic', and yellow for `toc'.
//
network Tictoc2
{
    submodules:
        tic: Txc2 {
            parameters:
                @display("i=block/process,cyan"); // specify an icon and colorize it
        };
        toc: Txc2 {
            parameters:
                @display("i=block/process,gold"); // here too
        };
    connections:
        tic.out --> { delay = 100ms; } --> toc.in;
        tic.in <-- { delay = 100ms; } <-- toc.out;
}

