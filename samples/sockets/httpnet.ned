//
// This file is part of an OMNeT++/OMNEST simulation example.
//
// Copyright (C) 1992-2005 Andras Varga
//
// This file is distributed WITHOUT ANY WARRANTY. See the file
// `license' for details on this and other legal matters.
//

//
// Model of the network, consisting of several clients, a server and a cloud
//
network HTTPNet
{
    parameters:
        int numClients = default(3) @prompt("Number of clients:");
    submodules:
        server: HTTPServer {
            parameters:
                @display("p=426,178");
        };
        cloud: Cloud {
            parameters:
                @display("p=274,182");
            gates:
                in[numClients+2];
                out[numClients+2];
        };
        extClient: ExtHTTPClient {
            parameters:
                addr = 1;
                srvAddr = 0;
                @display("p=102,92;t=Run the simulation\\, then\nenter into your browser:\nhttp://localhost:4242");
        };
        client[numClients]: HTTPClient {
            parameters:
                addr = 2+index;
                srvAddr = 0;
                @display("p=102,162,col");
        };
    connections:
        server.out --> cloud.in[0];
        server.in <-- cloud.out[0];
        extClient.out --> cloud.in[1];
        extClient.in <-- cloud.out[1];
        for i=0..numClients-1 {
            client[i].out --> cloud.in[i+2];
            client[i].in <-- cloud.out[i+2];
        };
}


