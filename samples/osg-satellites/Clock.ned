//
// This file is part of an OMNeT++/OMNEST simulation example.
//
// Copyright (C) 2015 OpenSim Ltd.
//
// This file is distributed WITHOUT ANY WARRANTY. See the file
// `license' for details on this and other legal matters.
//

//
// Generates periodic events to ensure the progress of simulation time
//
simple Clock
{
    parameters:
        double timeStep @unit("s") = default(60s);   // the time granularity of movement calculation in simulation time
        @display("i=block/cogwheel_s");
}
