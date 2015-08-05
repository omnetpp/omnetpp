//
// This file is part of an OMNeT++/OMNEST simulation example.
//
// Copyright (C) 2010 OpenSim Ltd.
//
// This file is distributed WITHOUT ANY WARRANTY. See the file
// `license' for details on this and other legal matters.
//

//
// A mobile node that travels at a constant speed, with the heading
// changing by a random value at each update.
//
simple RambleNode extends MobileNode
{
    parameters:
        @class(RambleNode);
        @display("i=block/broadcast");
        // mobility parameters
        double speed @unit("mps") = default(10mps);  // the speed of the mobile nodes
        double startX @unit("m") = default(0m);      // the starting coordinates in meter
        double startY @unit("m") = default(0m);      // the starting coordinates in meter
}
