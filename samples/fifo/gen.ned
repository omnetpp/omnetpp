//
// This file is part of an OMNeT++ simulation example.
//
// Copyright (C) 1992-2004 Andras Varga
//
// This file is distributed WITHOUT ANY WARRANTY. See the file
// `license' for details on this and other legal matters.
//


//
// Generates messages with the given interarrival time and length
//
simple FFGenerator
    parameters:
        sendIaTime : numeric,
        msgLength : numeric;
    gates:
        out: out;
endsimple


