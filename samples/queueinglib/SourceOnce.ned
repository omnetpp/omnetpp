//
// This file is part of an OMNeT++/OMNEST simulation example.
//
// Copyright (C) 2006-2008 OpenSim Ltd.
//
// This file is distributed WITHOUT ANY WARRANTY. See the file
// `license' for details on this and other legal matters.
//

package omnetpp.queueing;

//
// A module that generates a certain number of jobs once, at a given time.
// The name, type and priority of jobs can be set as well.
//
// This module is useful for injecting jobs into a closed queueing network.
//
simple SourceOnce extends Source
{
    parameters:
        @group(Queueing);
        @display("i=block/source");
        int numJobs = default(1);  // number of jobs to be generated
        double time @unit(s) = default(0s); // when to generate the jobs
        startTime = time;
        stopTime = -1s;
        interArrivalTime = 0;
}
