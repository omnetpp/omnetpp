//-------------------------------------------------------------
// file: gen1.ned
//        (part of Fifo1 - an OMNeT++ demo simulation)
//-------------------------------------------------------------


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

