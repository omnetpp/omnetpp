//-------------------------------------------------------------
// file: fifo.ned
//        (part of Fifo2 - an OMNeT++ demo simulation)
//-------------------------------------------------------------

// ACPFifo, ACBFifo: single-server queues in two flavours

simple ACPFifo
  parameters:
    service_time;
  gates:
    in: in;
    out: out;
endsimple

simple ACBFifo
  parameters:
    bits_per_sec;
  gates:
    in: in;
    out: out;
endsimple
