//
// This file is part of an OMNeT++/OMNEST simulation example.
//
// Copyright (C) 2015 OpenSim Ltd.
//
// This file is distributed WITHOUT ANY WARRANTY. See the file
// `license' for details on this and other legal matters.
//

//
// Represents a satellite. Loads the 3D model from the given file, and 
// moves it along the specified orbit.
//
simple Satellite
{
    parameters:
        @display("i=device/satellite");
        // 3D model parameters
        string modelURL;                               // the URL/filename of the 3D model to be used for the node
        double modelScale = default(1);
        // decorations and annotations
        string labelColor = default("#000000ff");      // the color of the model label in hex RRGGBBAA format or "" to turn off labels
        string orbitColor = default("#80C8C8C8");      // the color of the orbit circle in hex RRGGBBAA, or "" to hide the orbit
        string coneColor = default("#FFFFFF16");       // the color of the range cone in hex RRGGBBAA, or "" to hide the cone
        double altitude @unit("km") = default(5000km); // above sea level
        string orbitNormal = default("");              // can't be zero length, for example: "2.5,3,0", or leave it empty for a random orbit plane
        double startingPhase @unit("deg") = default(0deg);
}
