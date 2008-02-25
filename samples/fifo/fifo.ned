//
// This file is part of an OMNeT++/OMNEST simulation example.
//
// Copyright (C) 1992-2005 Andras Varga
//
// This file is distributed WITHOUT ANY WARRANTY. See the file
// `license' for details on this and other legal matters.
//


//
// Single-server queue, service rate independent of message length
//
simple PacketFifo
{
    parameters:
        volatile double serviceTime @unit(s);
        @display("i=block/queue;q=queue");
    gates:
        input in;
        output out;
}


//
// Single-server queue, service rate based on message length
//
simple BitFifo
{
    parameters:
        volatile double bitsPerSec @unit(bps);
        @display("i=block/queue;q=queue");
    gates:
        input in;
        output out;
}


