//
// This file is part of an OMNeT++/OMNEST simulation example.
//
// Copyright (C) 2003-2015 Andras Varga
//
// This file is distributed WITHOUT ANY WARRANTY. See the file
// `license' for details on this and other legal matters.
//


simple Tic7
    gates:
        in: in;
        out: out;
endsimple

simple Toc7
    gates:
        in: in;
        out: out;
endsimple

module Tictoc7
    submodules:
        tic: Tic7;
            display: "i=block/process,cyan";
        toc: Toc7;
            display: "i=block/process,gold";
    connections:
        tic.out --> delay 100ms --> toc.in;
        tic.in <-- delay 100ms <-- toc.out;
endmodule

network tictoc7 : Tictoc7
endnetwork

