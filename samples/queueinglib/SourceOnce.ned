//
// This file is part of an OMNeT++/OMNEST simulation example.
//
// Copyright (C) 2006-2015 OpenSim Ltd.
//
// This file is distributed WITHOUT ANY WARRANTY. See the file
// `license' for details on this and other legal matters.
//

package org.omnetpp.queueing;

//
// A module that generates a certain number of jobs once, at a given time.
// The name, type and priority of jobs can be set as well.
//
// This module is useful for injecting jobs into a closed queueing network.
//
simple SourceOnce
{
    parameters:
        @group(Queueing);
        @display("i=block/source");
        @signal[created](type="long");
        int numJobs = default(1);            // number of jobs to generate
        double time @unit(s) = default(0s);  // when to generate
        string jobName = default("job");     // the base name of the generated job (will be the module name if left empty)
        volatile int jobType = default(0);   // the type attribute of the created job (used by classifers and other modules)
        volatile int jobPriority = default(0);  // priority of the job
    gates:
        output out;
}


