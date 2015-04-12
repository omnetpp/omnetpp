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
import builder.NetBuilder;

network Dynamic
{
    submodules:
        builder: NetBuilder {
            nodesFile = "networks/nodes.txt";
            connectionsFile = "networks/connections.txt";
        }
}


