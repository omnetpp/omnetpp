//
// This file is part of an OMNeT++/OMNEST simulation example.
//
// Copyright (C) 2010 OpenSim Ltd.
//
// This file is distributed WITHOUT ANY WARRANTY. See the file
// `license' for details on this and other legal matters.
//

//
// A mobile node showing a 3D model. Able to display
// its position, trail, transmission range
//
simple Satellite
{
    parameters:
        @display("i=device/satellite");
        // 3D model parameters
        string modelURL;                             // the URL/filename of the 3D model to be used for the node
        double modelScale = default(1);
        // decorations and annotations
        string labelColor = default("#000000ff");    // the color of the model label in hex RRGGBBAA format or "" to turn off labels
        
        double altitude @unit("km") = default(5000km); // above sea level
        double startingPhase @unit("rad") = default(0rad);
        double timeStep @unit("s") = default(60s);  // the time granularity of movement calculation
}
