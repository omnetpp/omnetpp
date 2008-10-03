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
// Forks a job to one child per output. The children can be joined together
// into the original job with the Join module.
//
simple Split
{
    parameters:
        @display("i=block/fork");
    gates:
        input in;
        output out[];
}
