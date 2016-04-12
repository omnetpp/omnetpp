//
// This file is part of an OMNeT++/OMNEST simulation example.
//
// Copyright (C) 2015 OpenSim Ltd.
//
// This file is distributed WITHOUT ANY WARRANTY. See the file
// `license' for details on this and other legal matters.
//

//
// A mobile node that follows a track defined by a set of points
//
simple WaypointTrackerNode extends MobileNode
{
    parameters:
        @class(WaypointTrackerNode);
        @display("i=block/broadcast");
        double speed @unit("mps") = default(10mps);  // the speed of the mobile nodes
        double waypointProximity @unit("m") = default(5m);      // switch to the next waypoint if the current waypoint target is closer than this
        string trackFile;                            // a file with longitude latitude pairs
}
