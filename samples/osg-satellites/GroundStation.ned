//
// This file is part of an OMNeT++/OMNEST simulation example.
//
// Copyright (C) 2015 OpenSim Ltd.
//
// This file is distributed WITHOUT ANY WARRANTY. See the file
// `license' for details on this and other legal matters.
//

//
// Represents a satellite ground station at the given longitude/latitude.
//
simple GroundStation
{
    parameters:
        @display("i=device/receiverdish");
        // 3D model parameters
        string modelURL;                             // the URL/filename of the 3D model to be used for the node
        double modelScale = default(1);
        // decorations and annotations
        string labelColor = default("#000000ff");    // the color of the model label in hex RRGGBBAA format or "" to turn off labels
        double longitude;
        double latitude;
        string label;
        double timeStep @unit("s") = default(33ms);  // the time granularity of movement calculation (for 30fps)
        double startTime @unit("s") = default(0s);   // time when the movement starts
}
