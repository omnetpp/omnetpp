//-------------------------------------------------------------
// file: gen.ned
//        (part of Fifo2 - an OMNeT++ demo simulation)
//-------------------------------------------------------------


// FF2BurstyGenerator --
//
// generates messages (jobs,packets etc.)
//
simple FF2BurstyGenerator
    parameters:
        sleep_time_mean : numeric,
        burst_time_mean : numeric,
        send_ia_time : numeric,
        msg_length : numeric;
    gates:
        out: out;
endsimple


