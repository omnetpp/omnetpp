//
// This file is part of an OMNeT++/OMNEST simulation example.
//
// Copyright (C) 1992-2005 Andras Varga
//
// This file is distributed WITHOUT ANY WARRANTY. See the file
// `license' for details on this and other legal matters.
//


//
// Delays messages
//
simple QDelay
{
    parameters:
        volatile double delayBy;
        @display("i=block/delay");
    gates:
        output out;
        input in;
}


