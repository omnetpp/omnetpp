//
// This file is part of an OMNeT++/OMNEST simulation example.
//
// Copyright (C) 1992-2015 Andras Varga
//
// This file is distributed WITHOUT ANY WARRANTY. See the file
// `license' for details on this and other legal matters.
//

package networks;

import node.Node;
import ned.DatarateChannel;

//
// A ten-node network topology
//
network Net10
{
    types:
        channel C extends DatarateChannel
        {
            delay = default(uniform(0.1ms, 1ms));
            datarate = default(1Mbps);
        }
    submodules:
        rte[10]: Node {
            address = index;
        }
    connections:
        rte[1].port++ <--> C <--> rte[0].port++;
        rte[0].port++ <--> C <--> rte[2].port++;
        rte[2].port++ <--> C <--> rte[5].port++;
        rte[5].port++ <--> C <--> rte[7].port++;
        rte[7].port++ <--> C <--> rte[9].port++;
        rte[9].port++ <--> C <--> rte[8].port++;
        rte[8].port++ <--> C <--> rte[4].port++;
        rte[4].port++ <--> C <--> rte[3].port++;
        rte[3].port++ <--> C <--> rte[1].port++;
        rte[3].port++ <--> C <--> rte[2].port++;
        rte[2].port++ <--> C <--> rte[4].port++;
        rte[4].port++ <--> C <--> rte[6].port++;
        rte[6].port++ <--> C <--> rte[8].port++;
        rte[9].port++ <--> C <--> rte[6].port++;
        rte[6].port++ <--> C <--> rte[7].port++;
        rte[5].port++ <--> C <--> rte[6].port++;
        rte[0].port++ <--> C <--> rte[5].port++;
        rte[1].port++ <--> C <--> rte[2].port++;
}

