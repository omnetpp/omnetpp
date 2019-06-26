//
// This file is part of an OMNeT++/OMNEST simulation example.
//
// Copyright (C) 1992-2019 Andras Varga
//
// This file is distributed WITHOUT ANY WARRANTY. See the file
// `license' for details on this and other legal matters.
//


//
// Represents a place in a Petri net
//
simple Place
{
    parameters:
        int numInitialTokens = default(0);
        @display("i=place0");
        @signal[numTokens](type=long);
        @statistic[numTokens](record=vector;interpolationmode=sample-hold);
    gates:
        input in[];
        output out[];
}
