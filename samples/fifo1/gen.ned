//-------------------------------------------------------------
// file: gen.ned
//        (part of Fifo1 - an OMNeT++ demo simulation)
//-------------------------------------------------------------


// Generator --
//
// generates messages (jobs,packets etc.)
//
simple Generator
    parameters:
        num_messages, ia_time, msg_length;
    gates:
        out: out;
endsimple

