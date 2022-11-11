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
network Net5
{
    types:
        channel Link extends DatarateChannel
        {
            delay = uniform(0.01ms, 1s);
            datarate = uniform(1kbps, 100kbps);
        }
    submodules:
        rte[5]: Node {
            address = index;
        }
    connections:
        rte[1].port++ <--> Link <--> rte[0].port++;
        rte[0].port++ <--> Link <--> rte[2].port++;
        rte[4].port++ <--> Link <--> rte[3].port++;
        rte[3].port++ <--> Link <--> rte[1].port++;
        rte[3].port++ <--> Link <--> rte[2].port++;
        rte[2].port++ <--> Link <--> rte[4].port++;
        rte[1].port++ <--> Link <--> rte[2].port++;
}

