//
// This file is part of an OMNeT++ simulation example.
//
// Copyright (C) 2003-2004 Andras Varga
//
// This file is distributed WITHOUT ANY WARRANTY. See the file
// `license' for details on this and other legal matters.
//


simple Txc9
    gates:
        in: in[];  // declare in[] and out[] to be vector gates
        out: out[];
endsimple

module Tictoc9
    parameters:
        n: numeric const;
    submodules:
        tic: Txc9[n];  // we'll have n tic modules as well
            gatesizes:
                in[n], out[n];  // set the size of in[] and out[]
            display: "i=proc1;p=,,ring";  // arrange them in a ring
    connections nocheck:
        for i=0..n-2, j=i+1..n-1 do   // connect every tic to every other
            tic[i].out[j] --> delay 100ms --> tic[j].in[i];
            tic[i].in[j] <-- delay 100ms <-- tic[j].out[i];
        endfor;
endmodule

network tictoc9 : Tictoc9
endnetwork

