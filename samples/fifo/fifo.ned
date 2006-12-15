//
// This file is part of an OMNeT++/OMNEST simulation example.
//
// Copyright (C) 1992-2005 Andras Varga
//
// This file is distributed WITHOUT ANY WARRANTY. See the file
// `license' for details on this and other legal matters.
//


//
// Single-server queue, same service rate for each packet
//
simple FFPacketFifo
{
    parameters:
        volatile double serviceTime;
    gates:
        input in;
        output out;
}


//
// Single-server queue, service rate based on message length
//
simple FFBitFifo
{
    parameters:
        volatile double bitsPerSec;
        @display("i=block/queue");
    gates:
        input in;
        output out;
}


