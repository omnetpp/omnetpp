//
// This file is part of an OMNeT++ simulation example.
//
// Copyright (C) 1992-2003 Andras Varga
//
// This file is distributed WITHOUT ANY WARRANTY. See the file
// `license' for details on this and other legal matters.
//


//
// Bursty message source
//
simple PPSource
    parameters:
        sleepTimeMean : numeric,
        burstTimeMean : numeric,
        sendIaTime : numeric,
        msgLength : numeric;
    gates:
        out: out;
endsimple


