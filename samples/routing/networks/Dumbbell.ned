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

network Dumbbell
{
    parameters:
        int numLeft = default(9);
        int numRight = default(4);
    types:
        channel Link extends ned.DatarateChannel
        {
            delay = default(uniform(0.1ms, 1ms));
            datarate = default(1Mbps);
        }

    submodules:
        switchL: Node {address = 100; @display("p=200,200");}
        switchR: Node {address = 200; @display("p=500,200");}
        nodeL[numLeft]: Node {address = 101+index;}
        nodeR[numRight]: Node {address = 201+index;}
    connections:
        switchL.port++ <--> Link <--> switchR.port++;
        nodeL[i].port++ <--> Link <--> switchL.port++ for i=0..numLeft-1;
        nodeR[i].port++ <--> Link <--> switchR.port++ for i=0..numRight-1;
}


