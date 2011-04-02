//
// This file is part of an OMNeT++/OMNEST simulation example.
//
// Contributed by Nick van Foreest
//
// This file is distributed WITHOUT ANY WARRANTY. See the file
// `license' for details on this and other legal matters.
//


//
// Generates messages (jobs,packets etc.)
//
simple Generator
    parameters:
        num_messages: numeric,
        ia_time: numeric;
    gates:
        out: out;
endsimple

