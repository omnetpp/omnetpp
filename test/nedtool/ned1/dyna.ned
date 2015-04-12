//
// This file is part of an OMNeT++/OMNEST simulation example.
//
// Copyright (C) 1992-2015 Andras Varga
//
// This file is distributed WITHOUT ANY WARRANTY. See the file
// `license' for details on this and other legal matters.
//


//
// A client computer which periodically connects to the
// server for data exchange.
//
simple Client
    parameters:
        timeout: numeric const,
        connIaTime: numeric,
        queryIaTime: numeric,
        numQuery: numeric;
    gates:
        out: out;
        in: in;
endsimple


//
// A switch that accepts DynaPackets, and sends packets with destination
// address `a' to port out[a]. Incoming packets are queued up in a single
// queue and served `pkRate' packets per second. The queue has a finite
// size (overflow packets are dropped).
//
simple Switch
    parameters:
        pkRate: numeric const,
        queueMaxLen: numeric const;
    gates:
        out: out[];
        in: in[];
endsimple


//
// Accepts connections from the client computers.
// It serves multiple connections at a time; each connection is handled
// by a ServerProcess module created on demand.
//
simple Server
    parameters:
        processingTime: numeric const;
    gates:
        out: out;
        in: in;
endsimple


//
// Handles one connection in the server.
//
simple ServerProcess
    gates:
        in: in;
endsimple


//
// Model of the network, consisting of serveral clients, a server and a switch
//
module ClientServer
    parameters:
        numClients: numeric const;
    submodules:
        server: Server;
            parameters:
                processingTime = input(0.2s,"Query processing time:");
            display: "i=device/server;p=210,70";
        switch: Switch;
            parameters:
                // switchPkRate should be >= numClients, otherwise switch will
                // become the bottleneck
                pkRate = 1.5*numClients,
                // buffer max 20 packets
                queueMaxLen = 20;
            gatesizes:
                in[numClients+1],
                out[numClients+1];
            display: "i=device/switch;q=queue;p=210,170";
        client: Client[numClients];
            parameters:
                timeout = 5s,
                connIaTime = input(10s,"Connection interarrival time:"),
                queryIaTime = input(2s,"Query interarrival time:"),
                numQuery = input(5,"Number of queries per conn:");
            display: "i=device/pc2;p=70,270,m,10,80";
    connections:
        for i=0..numClients-1 do
            client[i].out --> delay 10ms --> switch.in[i];
            client[i].in <-- delay 10ms <-- switch.out[i];
        endfor;
        server.out --> delay 10ms --> switch.in[numClients];
        server.in <-- delay 10ms <-- switch.out[numClients];
endmodule


//
// Instantiates a ClientServer network.
//
network dyna : ClientServer
    parameters:
        numClients = input(4,"Number of clients:");
endnetwork


