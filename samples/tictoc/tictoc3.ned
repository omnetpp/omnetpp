//
// This file is part of an OMNeT++ simulation example.
//
// Copyright (C) 2003-2004 Andras Varga
//
// This file is distributed WITHOUT ANY WARRANTY. See the file
// `license' for details on this and other legal matters.
//


simple Txc3
    gates:
        in: in;
        out: out;
endsimple

//
// Same as Tictoc2.
//
module Tictoc3
    submodules:
        tic: Txc3;
            display: "i=proc1,cyan";
        toc: Txc3;
            display: "i=proc1,gold";
    connections:
        tic.out --> delay 100ms --> toc.in;
        tic.in <-- delay 100ms <-- toc.out;
endmodule

network tictoc3 : Tictoc3
endnetwork

