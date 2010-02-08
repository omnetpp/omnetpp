//
// This file is part of an OMNeT++/OMNEST simulation example.
//
// Copyright (C) 1992-2008 Andras Varga
//
// This file is distributed WITHOUT ANY WARRANTY. See the file
// `license' for details on this and other legal matters.
//


//
// Single-server queue, service rate independent of message length
//
simple Fifo
{
    parameters:
        volatile double serviceTime @unit(s);
        @display("i=block/queue;q=queue");
        @signal[qlen](title="queue length";modeHint=timeavg,max;interpolationmode=sample-hold);
        @signal[busy](title="server busy state";modeHint=timeavg;interpolationmode=sample-hold);
        @signal[queueingTime](title="queueing time at dequeue";unit=s;interpolationmode=none);
    gates:
        input in;
        output out;
}
