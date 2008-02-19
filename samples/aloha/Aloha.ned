//
// This file is part of an OMNeT++/OMNEST simulation example.
//
// Copyright (C) 1992-2005 Andras Varga
//
// This file is distributed WITHOUT ANY WARRANTY. See the file
// `license' for details on this and other legal matters.
//


//
// The Aloha network consists of hosts which talk to the central "server" via
// the Aloha or Slotted Aloha protocol
//
network Aloha
{
    parameters:
        int numHosts;  // number of hosts
        double txRate @unit(bps);  // transmission rate
        double slotTime;  // zero means no slots (pure Aloha)
        @display("bgb=,,white");
    submodules:
        server: Server {
            parameters:
                txRate = txRate;
        };
        host[numHosts]: Host {
            parameters:
                txRate = txRate;
                slotTime = slotTime;
        };
    connections allowunconnected:
}

