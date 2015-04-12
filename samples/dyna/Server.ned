//
// This file is part of an OMNeT++/OMNEST simulation example.
//
// Copyright (C) 1992-2015 Andras Varga
//
// This file is distributed WITHOUT ANY WARRANTY. See the file
// `license' for details on this and other legal matters.
//


//
// Accepts connections from the client computers.
// It serves multiple connections at a time; each connection is handled
// by a ServerProcess module created on demand.
//
simple Server
{
    parameters:
        double processingTime @unit(s) @prompt("Time to process a query") = default(0.2s);
        @display("i=device/server");
    gates:
        inout port;
}


