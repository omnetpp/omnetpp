//
// This file is part of an OMNeT++/OMNEST simulation example.
//
// Copyright (C) 1992-2005 Andras Varga
//
// This file is distributed WITHOUT ANY WARRANTY. See the file
// `license' for details on this and other legal matters.
//

package networks;

import node.Node;
import ned.DatarateChannel;

//
// A network topology commonly known as the "NTT backbone".
//
network Net10
{
    types:
        channel C extends DatarateChannel
        {
            parameters:
                delay = 0.1ms;
                datarate = 1Mbps;
        }
    submodules:
        rte[10]: Node {
            parameters:
                address = index;
        }
    connections allowunconnected:
        rte[0].port++ <--> C <--> rte[1].port++;
        rte[0].port++ <--> C <--> rte[3].port++;
        rte[1].port++ <--> C <--> rte[2].port++;
        rte[1].port++ <--> C <--> rte[4].port++;
        rte[3].port++ <--> C <--> rte[4].port++;
        rte[3].port++ <--> C <--> rte[8].port++;
        rte[4].port++ <--> C <--> rte[5].port++;
        rte[4].port++ <--> C <--> rte[7].port++;
        rte[5].port++ <--> C <--> rte[6].port++;
        rte[5].port++ <--> C <--> rte[1].port++;
        rte[6].port++ <--> C <--> rte[7].port++;
        rte[6].port++ <--> C <--> rte[9].port++;
        rte[7].port++ <--> C <--> rte[8].port++;
        rte[7].port++ <--> C <--> rte[2].port++;
        rte[9].port++ <--> C <--> rte[1].port++;
}

