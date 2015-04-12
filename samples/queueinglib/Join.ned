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
// Waits until all sub-jobs created by a Fork module arrive, then
// replaces them with the original job on the output.
//
simple Join
{
    @display("i=block/join");
    gates:
        input in[];
        output out;
}
