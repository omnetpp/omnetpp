//
// This file is part of an OMNeT++/OMNEST simulation example.
//
// Copyright (C) 2015 OpenSim Ltd.
//
// This file is distributed WITHOUT ANY WARRANTY. See the file
// `license' for details on this and other legal matters.
//

//
// This module is responsible for tracking the distance of mobile nodes,
// and visualizing the connectivity graph via KML.
//
simple ChannelController
{
    @display("i=block/network2");
    bool showConnections = default(true); // whether to show the connection graph (all nodes within range are connected by a line)
    string connectionColor = default(""); // the color of the connection graph in hex BBGGRR format or "" for random color
}
