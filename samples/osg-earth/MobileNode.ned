//
// This file is part of an OMNeT++/OMNEST simulation example.
//
// Copyright (C) 2015 OpenSim Ltd.
//
// This file is distributed WITHOUT ANY WARRANTY. See the file
// `license' for details on this and other legal matters.
//

//
// A mobile node showing a 3D model. Able to display
// its position, trail, transmission range
//
simple MobileNode
{
    parameters:
        @display("i=block/broadcast");
        string modelURL;                             // the URL/filename of the 3D model to be used for the node
                                                     // (you can use osg pseudo filters like: modelname.osgb.3.scale.0,0,45.rot scaleX3, rotate 45 deg around Z)
        string modelColor = default("");             // colorizes the 3D model used for the node
        // decorations and annotations
        string labelColor = default("#000000ff");    // the color of the model label in hex RRGGBBAA format or "" to turn off labels
        bool showTxRange = default(true);            // whether to show the transmission range around the nodes
        string rangeColor = default("#ff000040");    // the color of the range indicator in hex RRGGBBAA format
        string trailColor = default("#000000ff");    // the color of the trail indicator in hex RRGGBBAA format
        int trailLength = default(0);	             // the length of the trail in time steps showing the past movement of the node. 0 disables the trail
        double txRange @unit("m") = default(200m);   // the transmission range of the mobile node's radio
        double timeStep @unit("s") = default(33ms);  // the time granularity of movement calculation (for 30fps)
        double startTime @unit("s") = default(0s);   // time when the movement starts
}
