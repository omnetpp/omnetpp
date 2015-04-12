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
// A block that delays incoming messages by some time. The delay time is
// determined by the <code>delay</code> parameter which can be a distribution.
// Please note that the order of incoming and outgoing messages are NOT necessarily the same.
//
// @author rhornig
//
simple Delay
{
    parameters:
        @group(Queueing);
        @signal[delayedJobs](type="long");
        @statistic[delayedJobs](title="number of delayed jobs";record=vector?,timeavg,max;interpolationmode=sample-hold);
        @display("i=block/delay");
        volatile double delay @unit(s); // the requested delay time (can be a distribution)
    gates:
        input in[];                     // the incoming message gates
        output out;                     // outgoing message gate
}
