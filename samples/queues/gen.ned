//-------------------------------------------------------------
// file: gen.ned
//        (part of Queues - an OMNeT++ demo simulation)
//-------------------------------------------------------------


// Generator --
//
// generates messages (jobs,packets etc.)
//
simple Generator
    parameters:
        num_messages, ia_time;
    gates:
        out: out;
endsimple

