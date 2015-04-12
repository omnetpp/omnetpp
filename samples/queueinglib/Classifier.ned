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
// Sends the messages to different outputs depending on messages type or priority.
// If no output found for a message, it is sent to the "rest" gate.
// Messages with type = i will be sent to out[i] (if dispatchField = type).
// Messages with priority = i will be sent to out[i] (if dispatchField = priority).
//
// @author rhornig
//
simple Classifier
{
    parameters:
        @group(Queueing);
        @display("i=block/classifier");
        string dispatchField @enum("type","priority") = default("type");
    gates:
        input in[];
        output out[];
        output rest;     // messages that were not classified
}
