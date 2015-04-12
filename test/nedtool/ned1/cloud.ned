//
// This file is part of an OMNeT++/OMNEST simulation example.
//
// Copyright (C) 1992-2015 Andras Varga
//
// This file is distributed WITHOUT ANY WARRANTY. See the file
// `license' for details on this and other legal matters.
//

//
// Represents a "network cloud" between a server and its numerous clients.
// Delay modeling is very simple: constant delay.
//
simple Cloud
    parameters:
        propDelay: numeric const;
    gates:
        in: in[];
        out: out[];
endsimple

