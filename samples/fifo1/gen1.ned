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
        num_messages, ia_time, msg_length;
    gates:
        out: out;
endsimple

