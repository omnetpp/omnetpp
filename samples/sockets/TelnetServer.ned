//
// This file is part of an OMNeT++/OMNEST simulation example.
//
// Copyright (C) 1992-2005 Andras Varga
//
// This file is distributed WITHOUT ANY WARRANTY. See the file
// `license' for details on this and other legal matters.
//


//
// Telnet Server
//
simple TelnetServer
{
    parameters:
        volatile double serviceTime;
        @display("i=device/server");
    gates:
        inout g;
}



