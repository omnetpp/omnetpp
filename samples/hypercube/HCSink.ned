//
// This file is part of an OMNeT++/OMNEST simulation example.
//
// Copyright (C) 1992-2005 Andras Varga
//
// This file is distributed WITHOUT ANY WARRANTY. See the file
// `license' for details on this and other legal matters.
//


//
// Destroys packets and creates statistics
//
simple HCSink
{
    parameters:
        int address;
        @display("i=block/sink");
    gates:
        input in;
}


