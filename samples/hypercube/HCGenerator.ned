//
// This file is part of an OMNeT++/OMNEST simulation example.
//
// Copyright (C) 1992-2015 Andras Varga
//
// This file is distributed WITHOUT ANY WARRANTY. See the file
// `license' for details on this and other legal matters.
//


//
// Generates data packets
//
simple HCGenerator
{
    parameters:
        int numStations;
        int address;
        volatile double iaTime @unit(s);
        @display("i=block/source");
    gates:
        output out;
}


