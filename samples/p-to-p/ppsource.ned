//
// This file is part of an OMNeT++/OMNEST simulation example.
//
// Copyright (C) 1992-2005 Andras Varga
//
// This file is distributed WITHOUT ANY WARRANTY. See the file
// `license' for details on this and other legal matters.
//


//
// Bursty message source.
//
simple PPSource
    parameters:
        sleepTime : numeric, // sleep time between bursts
        burstTime : numeric, // duration of a burst
        sendIaTime : numeric, // time between generating packets during a burst
        msgLength : numeric; // length of a message, in bits
    gates:
        out: out;
endsimple


