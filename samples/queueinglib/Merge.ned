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
// Joins several incoming connections. Anything that comes in
// will be sent out immediately on the "out" gate.
//
simple Merge
{
    parameters:
        @group(Queueing);
        @display("i=block/join");
    gates:
        input in[];
        output out;
}
