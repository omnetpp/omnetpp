//
// This file is part of an OMNeT++ simulation example.
//
// Copyright (C) 1992-2003 Andras Varga
//
// This file is distributed WITHOUT ANY WARRANTY. See the file
// `license' for details on this and other legal matters.
//


//
// Generates messages (jobs, packets etc.)
//
simple FF1Generator
    parameters:
        num_messages, ia_time, msg_length;
    gates:
        out: out;
endsimple

