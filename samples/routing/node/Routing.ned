//
// This file is part of an OMNeT++/OMNEST simulation example.
//
// Copyright (C) 1992-2005 Andras Varga
//
// This file is distributed WITHOUT ANY WARRANTY. See the file
// `license' for details on this and other legal matters.
//

package node;

//
// Models a router.
//
simple Routing
{
    parameters:
        @display("i=block/switch");
    gates:
        input in[];
        output out[];
        input localIn;
        output localOut;
}


