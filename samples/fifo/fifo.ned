//
// This file is part of an OMNeT++/OMNEST simulation example.
//
// Copyright (C) 1992-2005 Andras Varga
//
// This file is distributed WITHOUT ANY WARRANTY. See the file
// `license' for details on this and other legal matters.
//


//
// Single-server queue, same service rate for each packet
//
simple FFPacketFifo
    parameters:
        serviceTime : numeric;
    gates:
        in: in;
        out: out;
endsimple


//
// Single-server queue, service rate based on message length
//
simple FFBitFifo
    parameters:
        bitsPerSec : numeric;
    gates:
        in: in;
        out: out;
endsimple


