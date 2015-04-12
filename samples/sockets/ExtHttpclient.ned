//
// This file is part of an OMNeT++/OMNEST simulation example.
//
// Copyright (C) 1992-2015 Andras Varga
//
// This file is distributed WITHOUT ANY WARRANTY. See the file
// `license' for details on this and other legal matters.
//


//
// A real-time interface
//
simple ExtHTTPClient
{
    parameters:
        int addr;
        int srvAddr;
        @display("i=device/pc2,gold");
    gates:
        inout g;
}



