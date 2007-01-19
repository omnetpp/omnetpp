//
// This file is part of an OMNeT++/OMNEST simulation example.
//
// Copyright (C) 1992-2005 Andras Varga
//
// This file is distributed WITHOUT ANY WARRANTY. See the file
// `license' for details on this and other legal matters.
//



//
// A "Node" consists of a Router plus an App module which represents the
// aggregate traffic of the subnets connected to that router.
//
module Node
{
    parameters:
        int address;
        @display("i=misc/node_vs,gold");
    gates:
        inout port[];
    submodules:
        app: App {
            parameters:
                @display("p=90,56");
        };
        routing: Routing {
            parameters:
                @display("p=90,132");
            gates:
                in[sizeof(port)];
                out[sizeof(port)];
        };
    connections:
        routing.localOut --> app.in;
        routing.localIn <-- app.out;
        for i=0..sizeof(port)-1 {
            routing.out[i] --> port$o[i];
            routing.in[i] <-- port$i[i];
        };
}


