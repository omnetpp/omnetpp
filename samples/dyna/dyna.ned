//
// This file is part of an OMNeT++/OMNEST simulation example.
//
// Copyright (C) 1992-2005 Andras Varga
//
// This file is distributed WITHOUT ANY WARRANTY. See the file
// `license' for details on this and other legal matters.
//


//
// A client computer which periodically connects to the
// server for data exchange.
//
simple Client
{
    parameters:
        double timeout;
        volatile double connIaTime;
        volatile double queryIaTime;
        volatile int numQuery;
        @display("i=device/pc2");
    gates:
        output out;
        input in;
}


//
// A switch that accepts DynaPackets, and sends packets with destination
// address `a' to port out[a]. Incoming packets are queued up in a single
// queue and served `pkRate' packets per second. The queue has a finite
// size (overflow packets are dropped).
//
simple Switch
{
    parameters:
        double pkRate;
        int queueMaxLen;
        @display("i=block/switch;q=queue");
    gates:
        output out[];
        input in[];
}


//
// Accepts connections from the client computers.
// It serves multiple connections at a time; each connection is handled
// by a ServerProcess module created on demand.
//
simple Server
{
    parameters:
        double processingTime;
        @display("i=device/server");
    gates:
        output out;
        input in;
}


//
// Handles one connection in the server.
//
simple ServerProcess
{
    gates:
        input in;
}


//
// Model of the network, consisting of serveral clients, a server and a switch
//
module ClientServer
{
    parameters:
        int numClients = default(4) @prompt("Number of clients:");
    submodules:
        server: Server {
            parameters:
                processingTime = default(0.2s) @prompt("Query processing time:");
                @display("p=210,70");
        };
        switch: Switch {
            parameters:
                // switchPkRate should be >= numClients, otherwise switch will
                // become the bottleneck
                pkRate = 1.5*numClients;
                // buffer max 20 packets
                queueMaxLen = 20;
                @display("p=210,170");
            gates:
                in[numClients+1];
                out[numClients+1];
        };
        client[numClients]: Client {
            parameters:
                timeout = 5s;
                connIaTime = default(10s) @prompt("Connection interarrival time:");
                queryIaTime = default(2s) @prompt("Query interarrival time:");
                numQuery = default(5) @prompt("Number of queries per conn:");
                @display("p=70,270,m,10,80");
        };
    connections:
        for i=0..numClients-1 {
            client[i].out --> {  delay = 10ms; } --> switch.in[i];
            client[i].in <-- {  delay = 10ms; } <-- switch.out[i];
        };
        server.out --> {  delay = 10ms; } --> switch.in[numClients];
        server.in <-- {  delay = 10ms; } <-- switch.out[numClients];
}


//
// Instantiates a ClientServer network.
//
network dyna extends ClientServer
{
}


