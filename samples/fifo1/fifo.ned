//-------------------------------------------------------------
// file: fifo.ned
//        (part of Fifo1 - an OMNeT++ demo simulation)
//-------------------------------------------------------------


// ACPFifo --
//
// single-server queue
//
simple ACPFifo
  parameters:
    service_time;
  gates:
    in: in;
    out: out;
endsimple


// ACPFifo --
//
// single-server queues in another flavour
//
simple ACBFifo
  parameters:
    bits_per_sec;
  gates:
    in: in;
    out: out;
endsimple
