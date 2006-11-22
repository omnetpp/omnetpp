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
{
    parameters:
        volatile double sleepTime; // sleep time between bursts
        volatile double burstTime; // duration of a burst
        volatile double sendIaTime; // time between generating packets during a burst
        volatile double msgLength; // length of a message, in bits
    gates:
        output out;
}


