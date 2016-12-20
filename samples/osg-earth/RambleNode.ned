//
// This file is part of an OMNeT++/OMNEST simulation example.
//
// Copyright (C) 2015 OpenSim Ltd.
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

        // transmission parameters
        volatile double transmitInterval @unit("s") = default(uniform(1s, 5s)); // the amount of time between transmissions
        double transmissionDuration @unit("s") = default(0.1s); // the duration of the transmission animation

        // appearance parameters
        string transmissionColor = default("grey");  // the color of the transmission animation (inflating sphere)
        double transmissionAlpha = default(0.5);     // the initial opacity of the transmission animation
}
