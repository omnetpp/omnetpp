//
// This file is part of an OMNeT++/OMNEST simulation example.
//
// Copyright (C) 1992-2005 Andras Varga
//
// This file is distributed WITHOUT ANY WARRANTY. See the file
// `license' for details on this and other legal matters.
//


// FF1Generator --
//
// generates messages (jobs,packets etc.)
//
simple FF1Generator
    parameters:
        messageKind : numeric const,
        arrivalRate : numeric;
    gates:
        out: out;
endsimple

