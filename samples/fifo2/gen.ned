//-------------------------------------------------------------
// file: gen.ned
//        (part of Fifo2 - an OMNeT++ demo simulation)
//-------------------------------------------------------------

// BurstyGenerator: generates messages (jobs,packets etc.)

simple BurstyGenerator
  parameters:
    sleep_time_mean,
    burst_time_mean,
    send_ia_time,
    msg_length;
  gates:
    out: out;
endsimple

