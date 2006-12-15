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
        int numMessages;
        volatile double iaTime;
        @display("i=block/source");
    gates:
        output out;
}

