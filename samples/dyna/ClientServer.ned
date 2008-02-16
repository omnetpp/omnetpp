//
// This file is part of an OMNeT++/OMNEST simulation example.
//
// Copyright (C) 1992-2005 Andras Varga
//
// This file is distributed WITHOUT ANY WARRANTY. See the file
// `license' for details on this and other legal matters.
//

//
// Model of the network, consisting of serveral clients, a server and a switch
//
network ClientServer
{
    parameters:
        int numClients = default(4) @prompt("Number of clients");
    submodules:
        server: Server {
            parameters:
                processingTime = default(0.2s) @prompt("Time to process a query");
                @display("p=210,70");
        };
        switch: Switch {
            parameters:
                // pkRate should be >= numClients, otherwise switch will become the bottleneck
                pkRate = 1.5*numClients;
                // buffer max 20 packets
                queueMaxLen = 20;
                @display("p=210,170");
            gates:
                port[numClients+1];
        };
        client[numClients]: Client {
            parameters:
                timeout = 5s;
                connIaTime = default(10s) @prompt("Connection interarrival time");
                queryIaTime = default(2s) @prompt("Query interarrival time");
                numQuery = default(5) @prompt("Number of queries per connection");
                @display("p=70,270,m,10,80");
        };
    connections:
        for i=0..numClients-1 {
            client[i].port <--> {delay = 10ms;} <--> switch.port[i];
        };
        server.port <--> {delay = 10ms;} <--> switch.port[numClients];
}


