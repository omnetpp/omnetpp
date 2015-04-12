//
// This file is part of an OMNeT++/OMNEST simulation example.
//
// Copyright (C) 1992-2015 Andras Varga
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
        double timeout @unit(s) @prompt("Connection timeout");
        volatile double connIaTime @unit(s) @prompt("Connection interarrival time") = default(10s);
        volatile double queryIaTime @unit(s) @prompt("Query interarrival time") = default(2s);
        volatile int numQuery @prompt("Number of queries per connection") = default(5);
        @display("i=device/pc2");
    gates:
        inout port;
}


