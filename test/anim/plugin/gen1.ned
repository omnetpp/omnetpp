//
// This file is part of an OMNeT++/OMNEST simulation example.
//
// Copyright (C) 1992-2015 Andras Varga
//
// This file is distributed WITHOUT ANY WARRANTY. See the file
// `license' for details on this and other legal matters.
//


//
// generates messages (jobs,packets etc.)
//
simple FF1Generator
{
    parameters:
        double messageKind;
        volatile double arrivalRate;
    gates:
        output out;
}

