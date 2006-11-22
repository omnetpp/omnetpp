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
{
    parameters:
        double num_messages;
        volatile double ia_time;
    gates:
        output out;
}

