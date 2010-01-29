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
        @signal[drop](title="dropped packet byte length";unit=bytes;modeHint=count,sum);
        @signal[outputIf](title="output interface for each routed packet";modeHint=histogram);
    gates:
        input in[];
        output out[];
        input localIn;
        output localOut;
}


