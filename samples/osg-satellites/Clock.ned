//
// This file is part of an OMNeT++/OMNEST simulation example.
//
// Copyright (C) 2015 OpenSim Ltd.
//
// This file is distributed WITHOUT ANY WARRANTY. See the file
// `license' for details on this and other legal matters.
//

//
// Generates periodic events. Since updating the 3D scene is all done
// inside refreshDisplay() methods that are only called by the GUI
// "when needed" (and there are no events in other modules), we need
// to artificially generate periodic events to get an animation.
// See the documentation of refreshDisplay() in the Simulation Manual.
//
simple Clock
{
    parameters:
        double timeStep @unit("s") = default(60s);   // the time granularity of movement calculation in simulation time
        @display("i=block/cogwheel_s");
}
