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
{
    parameters:
        string destAddresses;
    gates:
        input in;
        output out;
}


//
// Models a router.
//
simple Routing
{
    gates:
        input in[];
        output out[];
        input localIn;
        output localOut;
}


//
// A "Node" consists of a Router plus an App module which represents the
// aggregate traffic of the subnets connected to that router.
//
module Node
{
    parameters:
        double address;
    gates:
        input in[];
        output out[];
    submodules:
        app: App {
            parameters:
                @display("p=90,56;i=block/browser");
        };
        routing: Routing {
            parameters:
                @display("p=90,132;i=block/switch");
            gates:
                in[sizeof(in)];
                out[sizeof(out)];
        };
    connections:
        routing.localOut --> app.in;
        routing.localIn <-- app.out;
        for i=0..sizeof(in)-1 {
            routing.out[i] --> out[i];
            routing.in[i] <-- in[i];
        };
}


