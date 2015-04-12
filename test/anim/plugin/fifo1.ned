//
// This file is part of an OMNeT++/OMNEST simulation example.
//
// Copyright (C) 1992-2015 Andras Varga
//
// This file is distributed WITHOUT ANY WARRANTY. See the file
// `license' for details on this and other legal matters.
//


//
// single-server queue, same service rate for each packet
//
simple FF1PacketFifo
{
    parameters:
        volatile double serviceRate;
    gates:
        input in[];
        output out;
}


