//
// This file is part of an OMNeT++/OMNEST simulation example.
//
// Copyright (C) 1992-2019 Andras Varga
//
// This file is distributed WITHOUT ANY WARRANTY. See the file
// `license' for details on this and other legal matters.
//


//
// Represents a transition in a Petri net.
// Guard condition is to be expressed in C++.
//
simple Transition
{
    parameters:
        volatile double transitionTime @unit(s) = default(0s);
        string transitionSchedulerModule = default("scheduler");
        bool animation = default(true); // note: turning on animation introduces extra simulation events
        @display("i=transition,,60");
        @signal[firing](type=long);
        @statistic[firing](record=vector;interpolationmode=sample-hold);
    gates:
        input in[];
        output out[];
}
