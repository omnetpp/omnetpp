//
// This file is part of an OMNeT++/OMNEST simulation example.
//
// Copyright (C) 1992-2005 Andras Varga
//
// This file is distributed WITHOUT ANY WARRANTY. See the file
// `license' for details on this and other legal matters.
//


//
// A client computer which periodically connects to the
// server for data exchange.
//
simple Client
{
    parameters:
        double timeout @unit(s);
        volatile double connIaTime @unit(s);
        volatile double queryIaTime @unit(s);
        volatile int numQuery;
        @display("i=device/pc2");
    gates:
        inout port;
}


