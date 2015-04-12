//
// This file is part of an OMNeT++/OMNEST simulation example.
//
// Copyright (C) 2003-2015 Andras Varga
//
// This file is distributed WITHOUT ANY WARRANTY. See the file
// `license' for details on this and other legal matters.
//


simple Txc4
    parameters:
        limit: numeric const;  // declare the new parameter
    gates:
        in: in;
        out: out;
endsimple

//
// Adding module parameters.
//
module Tictoc4
    submodules:
        tic: Txc4;
            parameters:
                limit = 8;  // tic's limit is 8
            display: "i=block/process,cyan";
        toc: Txc4;
            // note that we leave toc's limit unbound here
            display: "i=block/process,gold";
    connections:
        tic.out --> delay 100ms --> toc.in;
        tic.in <-- delay 100ms <-- toc.out;
endmodule

network tictoc4 : Tictoc4
endnetwork

