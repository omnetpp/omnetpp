//
// This file is part of an OMNeT++/OMNEST simulation example.
//
// Copyright (C) 2015 OpenSim Ltd.
//
// This file is distributed WITHOUT ANY WARRANTY. See the file
// `license' for details on this and other legal matters.
//

//
// The osgEarth Demo network.
//
network OsgEarthNet
{
    parameters:
        @display("bgb=$playgroundWidth,$playgroundHeight;bgi=background/terrain,s");
        double playgroundLatitude; // geographic position of the playground's north-west corner
        double playgroundLongitude; // geographic position of the playground's north-west corner
        double playgroundWidth @unit("m") = default(300m);  // the E-W size of playground
        double playgroundHeight @unit("m") = default(300m); // the N-S size of playground
        int numCows = default(0);  // the number of cows wandering around on the Boston Commons
        int numCars = default(0);  // the number of cars on the streets

    submodules:
        osgEarthScene: OsgEarthScene {
            @display("p=74,31");
        }
        channelController: ChannelController {
            @display("p=243,31");
        }
        cow[numCows]: RambleNode {
            @display("p=48,150");
        }
        car[numCars]: WaypointTrackerNode {
            @display("p=243,150");
        }
}
