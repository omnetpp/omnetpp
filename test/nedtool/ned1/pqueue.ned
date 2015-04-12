//
// This file is part of an OMNeT++/OMNEST simulation example.
//
// Copyright (C) 1992-2015 Andras Varga
//
// This file is distributed WITHOUT ANY WARRANTY. See the file
// `license' for details on this and other legal matters.
//


//
// Passive queue. Messages have to be requested via direct method call.
//
simple QPassiveQueue
    gates:
        in: in;
        out: out[];
endsimple


