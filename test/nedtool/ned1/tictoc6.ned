//
// This file is part of an OMNeT++/OMNEST simulation example.
//
// Copyright (C) 2003-2015 Andras Varga
//
// This file is distributed WITHOUT ANY WARRANTY. See the file
// `license' for details on this and other legal matters.
//


simple Txc6
    parameters:
        delayTime: numeric;   // delay before sending back message
    gates:
        in: in;
        out: out;
endsimple

module Tictoc6
    submodules:
        tic: Txc6;
            display: "i=block/process,cyan";
        toc: Txc6;
            display: "i=block/process,gold";
    connections:
        tic.out --> delay 100ms --> toc.in;
        tic.in <-- delay 100ms <-- toc.out;
endmodule

network tictoc6 : Tictoc6
endnetwork

