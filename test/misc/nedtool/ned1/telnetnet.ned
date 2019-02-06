//
// This file is part of an OMNeT++/OMNEST simulation example.
//
// Copyright (C) 1992-2015 Andras Varga
//
// This file is distributed WITHOUT ANY WARRANTY. See the file
// `license' for details on this and other legal matters.
//


import
    "telnetclient",
    "exttelnetclient",
    "telnetserver",
    "cloud";


//
// Model of the network, consisting of several clients, a server and a cloud
//
module TelnetNet
    parameters:
        numClients: numeric const;
    submodules:
        server: TelnetServer;
            display: "i=device/server;p=426,166";
        cloud: Cloud;
            gatesizes:
                in[numClients+2],
                out[numClients+2];
            display: "i=misc/cloud;p=274,170";
        extClient: ExtTelnetClient;
            parameters:
                addr = 1,
                srvAddr = 0;
            display: "i=device/pc2,gold;p=102,80";
        client: TelnetClient[numClients];
            parameters:
                addr = 2+index,
                srvAddr = 0;
            display: "i=device/pc2;p=102,150,col";
    connections:
        server.out --> cloud.in[0];
        server.in <-- cloud.out[0];
        extClient.out --> cloud.in[1];
        extClient.in <-- cloud.out[1];
        for i=0..numClients-1 do
            client[i].out --> cloud.in[i+2];
            client[i].in <-- cloud.out[i+2];
        endfor;
endmodule


//
// Instantiates TelnetNet
//
network telnetNet : TelnetNet
    parameters:
        numClients = input(3,"Number of clients:");
endnetwork


