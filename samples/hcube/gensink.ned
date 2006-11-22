//
// This file is part of an OMNeT++/OMNEST simulation example.
//
// Copyright (C) 1992-2005 Andras Varga
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
        double num_stations;
        double address;
        volatile double ia_time;
    gates:
        output out;
}


//
// Destroys packets and creates statistics
//
simple HCSink
{
    parameters:
        double address;
    gates:
        input in;
}


