//
// This file is part of an OMNeT++/OMNEST simulation example.
//
// Copyright (C) 1992-2008 Andras Varga
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
        @signal[drop](title="dropped packet byte length";unit=bytes;modehint=count,sum;interpolationmode=none);
        @signal[outputIf](title="output interface for each routed packet";modehint=histogram;interpolationmode=none);
    gates:
        input in[];
        output out[];
        input localIn;
        output localOut;
}


