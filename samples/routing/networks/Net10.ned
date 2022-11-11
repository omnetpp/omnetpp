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
        channel Link extends DatarateChannel
        {
            delay = default(uniform(0.1ms, 1ms));
            datarate = default(1Mbps);
        }
    submodules:
        rte[10]: Node {
            address = index;
        }
    connections:
        rte[1].port++ <--> Link <--> rte[0].port++;
        rte[0].port++ <--> Link <--> rte[2].port++;
        rte[2].port++ <--> Link <--> rte[5].port++;
        rte[5].port++ <--> Link <--> rte[7].port++;
        rte[7].port++ <--> Link <--> rte[9].port++;
        rte[9].port++ <--> Link <--> rte[8].port++;
        rte[8].port++ <--> Link <--> rte[4].port++;
        rte[4].port++ <--> Link <--> rte[3].port++;
        rte[3].port++ <--> Link <--> rte[1].port++;
        rte[3].port++ <--> Link <--> rte[2].port++;
        rte[2].port++ <--> Link <--> rte[4].port++;
        rte[4].port++ <--> Link <--> rte[6].port++;
        rte[6].port++ <--> Link <--> rte[8].port++;
        rte[9].port++ <--> Link <--> rte[6].port++;
        rte[6].port++ <--> Link <--> rte[7].port++;
        rte[5].port++ <--> Link <--> rte[6].port++;
        rte[0].port++ <--> Link <--> rte[5].port++;
        rte[1].port++ <--> Link <--> rte[2].port++;
}

