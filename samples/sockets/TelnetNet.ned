//
// This file is part of an OMNeT++/OMNEST simulation example.
//
// Copyright (C) 1992-2015 Andras Varga
//
// This file is distributed WITHOUT ANY WARRANTY. See the file
// `license' for details on this and other legal matters.
//

//
// Model of the network, consisting of several clients, a server and a cloud
//
network TelnetNet
{
    parameters:
        int numClients @prompt("Number of clients") = default(3);
    submodules:
        server: TelnetServer {
            parameters:
                @display("p=426,178");
        }
        cloud: Cloud {
            parameters:
                @display("p=274,182");
            gates:
                g[parent.numClients+2];
        }
        extClient: ExtTelnetClient {
            parameters:
                addr = 1;
                srvAddr = 0;
                @display("p=102,92;t=Run the simulation\\, then start:\ntelnet localhost 4242\nThen if on Unix\\, type:\nCtrl+] ``mode character''");
        }
        client[numClients]: TelnetClient {
            parameters:
                addr = 2+index;
                srvAddr = 0;
                @display("p=102,162,col");
        }
    connections:
        server.g <--> cloud.g[0];
        extClient.g <--> cloud.g[1];
        client[i].g <--> cloud.g[i+2] for i=0..numClients-1;
}


