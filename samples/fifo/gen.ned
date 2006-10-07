//
// This file is part of an OMNeT++/OMNEST simulation example.
//
// Copyright (C) 1992-2005 Andras Varga
//
// This file is distributed WITHOUT ANY WARRANTY. See the file
// `license' for details on this and other legal matters.
//


//
// Generates jobs (messages) with the given interarrival time and length.
// Both can be random variates, returning different values for each job.
//
simple FFGenerator
    parameters:
        sendIaTime : numeric,
        msgLength : numeric;
    gates:
        out: out;
endsimple


