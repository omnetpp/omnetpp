//
// This file is part of an OMNeT++/OMNEST simulation example.
//
// Copyright (C) 1992-2005 Andras Varga
//
// This file is distributed WITHOUT ANY WARRANTY. See the file
// `license' for details on this and other legal matters.
//



//
// Application model to generate traffic for the network.
//
simple App
    parameters:
        destAddresses : string;
    gates:
        in: in;
        out: out;
endsimple


//
// Models a router.
//
simple Routing
    gates:
        in: in[];
        out: out[];
        in: localIn;
        out: localOut;
endsimple


//
// A "Node" consists of a Router plus an App module which represents the
// aggregate traffic of the subnets connected to that router.
//
module Node
    parameters:
        address : numeric;
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


