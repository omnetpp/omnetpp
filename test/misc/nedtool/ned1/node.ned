//
// This file is part of an OMNeT++/OMNEST simulation example.
//
// Copyright (C) 1992-2015 Andras Varga
//
// This file is distributed WITHOUT ANY WARRANTY. See the file
// `license' for details on this and other legal matters.
//


simple App
    parameters:
        destAddresses: string;
    gates:
        in: in;
        out: out;
endsimple

simple Routing
    gates:
        in: in[];
        out: out[];
        in: localIn;
        out: localOut;
endsimple

module Node
    parameters:
        address: numeric;
    gates:
        in: in[];
        out: out[];
    submodules:
        app: App;
            display: "p=90,56;i=block/browser";
        routing: Routing;
            gatesizes:
                in[sizeof(in)],
                out[sizeof(out)];
            display: "p=90,132;i=block/switch";
    connections:
        routing.localOut --> app.in;
        routing.localIn <-- app.out;
        for i=0..sizeof(in)-1 do
            routing.out[i] --> out[i];
            routing.in[i] <-- in[i];
        endfor;
endmodule


