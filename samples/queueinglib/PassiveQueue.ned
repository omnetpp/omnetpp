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
// Passive queue. Messages have to be requested via direct method call.
// Its output must be connected to a Server.
//
// @author rhornig
// @todo minDelay not implemented
//
simple PassiveQueue
{
    parameters:
        @group(Queueing);
        @display("i=block/passiveq;q=queue");
        int capacity = default(-1);  // negative capacity means unlimited queue
        bool fifo = default(true);   // whether the module works as a queue (fifo=true) or a stack (fifo=false)
        string sendingAlgorithm @enum("priority","random","roundRobin","minDelay") = default("priority");
                                     // how the queue tries to find a suitable server for an incoming job
    gates:
        input in[];
        output out[];
}
