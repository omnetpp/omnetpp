//
// This file is part of an OMNeT++/OMNEST simulation example.
//
// Copyright (C) 2015 OpenSim Ltd.
//
// This file is distributed WITHOUT ANY WARRANTY. See the file
// `license' for details on this and other legal matters.
//

//
// A mobile node showing a 3D model and positioning/orienting it in the scene graph.
//
simple Person
{
    parameters:
        @display("i=abstract/person");
        string modelURL;                             // the URL/filename of the 3D model to be used for the node
                                                     // (you can use osg pseudo filters like: modelname.osgb.3.scale.0,0,45.rot scaleX3, rotate 45 deg around Z)
        // decorations and annotations
        string labelColor = default("#000000ff");    // the color of the model label in hex RRGGBBAA format or "" to turn off labels
        double timeStep @unit("s") = default(33ms);  // the time granularity of movement calculation (for 30fps)
        double startTime @unit("s") = default(0s);   // time when the movement starts
        double speed @unit("mps") = default(10mps);  // the speed of the mobile nodes
        double waypointProximity @unit("m") = default(5m);      // switch to the next waypoint if the current waypoint target is closer than this
        string trackFile;                            // a file with x,y coordinate pairs
}
