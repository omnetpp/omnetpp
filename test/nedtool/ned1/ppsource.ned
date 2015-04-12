//
// This file is part of an OMNeT++/OMNEST simulation example.
//
// Copyright (C) 1992-2015 Andras Varga
//
// This file is distributed WITHOUT ANY WARRANTY. See the file
// `license' for details on this and other legal matters.
//


//
// Bursty message source
//
simple PPSource
    parameters:
        sleepTime: numeric,
        burstTime: numeric,
        sendIaTime: numeric,
        msgLength: numeric;
    gates:
        out: out;
endsimple


