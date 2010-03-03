//
// This file is part of an OMNeT++/OMNEST simulation example.
//
// Copyright (C) 2006-2008 OpenSim Ltd.
//
// This file is distributed WITHOUT ANY WARRANTY. See the file
// `license' for details on this and other legal matters.
//

package org.omnetpp.queueing;

//
// Destroys (or optionally keep) the packets and collects statistics
//
simple Sink
{
    parameters:
        @group(Queueing);
        @display("i=block/sink");
        @statistic[lifeTime](title="lifetime of arrived jobs";unit=s;interpolationmode=none);
        @statistic[totalQueueingTime](title="the total time spent in queues by arrived jobs";unit=s;interpolationmode=none);
        @statistic[totalDelayTime](title="the total time spent in delay nodes by arrived jobs";unit=s;interpolationmode=none);
        @statistic[totalServiceTime](title="the total time spent  by arrived jobs";unit=s;interpolationmode=none);
        @statistic[queuesVisited](title="the total number of queues visited by arrived jobs";interpolationmode=none);
        @statistic[delaysVisited](title="the total number of delays visited by arrived jobs";interpolationmode=none);
        @statistic[generation](title="the generation of the arrived jobs";interpolationmode=none);
        bool keepJobs = default(false); // whether to keep the received jobs till the end of simulation
    gates:
        input in[];
}

