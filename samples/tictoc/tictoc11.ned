//
// This file is part of an OMNeT++ simulation example.
//
// Copyright (C) 2003-2004 Andras Varga
//
// This file is distributed WITHOUT ANY WARRANTY. See the file
// `license' for details on this and other legal matters.
//


simple Txc11
    gates:
        in: in[];
        out: out[];
endsimple

//
// Same as Tictoc10.
//
module Tictoc11
    parameters:
        n: numeric const;
    submodules:
        tic: Txc11[n];
            gatesizes:
                in[n], out[n];
            display: "i=proc1;p=,,ring";
    connections nocheck:
        for i=0..n-2, j=i+1..n-1 do
            tic[i].out[j] --> delay 110ms --> tic[j].in[i];
            tic[i].in[j] <-- delay 110ms <-- tic[j].out[i];
        endfor;
endmodule

network tictoc11 : Tictoc11
endnetwork

