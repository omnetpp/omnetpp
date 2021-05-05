//
// This file is part of an OMNeT++/OMNEST simulation example.
//
// Copyright (C) 1992-2015 Andras Varga
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
        int numClients @prompt("Number of clients") = default(4);
    submodules:
        server: Server {
            parameters:
                @display("p=210,70");
        }
        switch: Switch {
            parameters:
                pkRate = 1.5*parent.numClients; // pkRate should be >= numClients, otherwise switch will become the bottleneck
                queueMaxLen = 20; // buffer max 20 packets
                @display("p=210,170");
            gates:
                port[parent.numClients+1];
        }
        client[numClients]: Client {
            parameters:
                timeout = 5s;
                @display("p=70,270,m,10,80");
        }
    connections:
        for i=0..numClients-1 {
            client[i].port <--> {  delay = 10ms; } <--> switch.port[i];
        }
        server.port <--> {  delay = 10ms; } <--> switch.port[numClients];
}


