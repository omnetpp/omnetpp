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
// Sends out incoming messages on ALL output gates. The original
// message is sent out on out[0]. The other output gates will get a copy
// of the original message.
//
simple Clone
{
    parameters:
        @group(Queueing);
        @display("i=block/fork");
        bool changeMsgNames = default(true);  // whether to change the message names when they go through the module
    gates:
        input in[];
        output out[];
}
