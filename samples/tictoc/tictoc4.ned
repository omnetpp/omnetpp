//
// This file is part of an OMNeT++ simulation example.
//
// Copyright (C) 2003-2004 Andras Varga
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
// Adding parameters.
//
module Tictoc4
    submodules:
        tic: Txc4;
            parameters:
                limit = 8;  // tic's limit is 8
            display: "i=proc1,cyan";
        toc: Txc4;
            parameters:
                limit = 6;  // toc's limit is 6 (so toc'll stop the simulation
                            // before tic would)
            display: "i=proc1,gold";
    connections:
        tic.out --> delay 100ms --> toc.in;
        tic.in <-- delay 100ms <-- toc.out;
endmodule

network tictoc4 : Tictoc4
endnetwork

