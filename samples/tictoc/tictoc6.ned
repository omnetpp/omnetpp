//
// This file is part of an OMNeT++ simulation example.
//
// Copyright (C) 2003-2004 Andras Varga
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
            parameters:
               delayTime = exponential(3.0);  // mean=3.0
            display: "i=proc1,cyan";
        toc: Txc6;
            // for `toc', we leave the delayTime parameter unassigned here and put
            // the value into omnetpp.ini instead, to make it easier to change.
            display: "i=proc1,gold";
    connections:
        tic.out --> delay 100ms --> toc.in;
        tic.in <-- delay 100ms <-- toc.out;
endmodule

network tictoc6 : Tictoc6
endnetwork

