//
// This file is part of an OMNeT++/OMNEST simulation example.
//
// Copyright (C) 1992-2015 Andras Varga
//
// This file is distributed WITHOUT ANY WARRANTY. See the file
// `license' for details on this and other legal matters.
//


//
// Generates jobs (messages) with the given interarrival time,
// which can be a random variate.
//
simple Source
{
    parameters:
        @pythonClass;
        volatile double interarrivalTime @unit(s);
        @display("i=block/source");
    gates:
        output out;
}


