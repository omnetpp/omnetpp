//-------------------------------------------------------------
// file: fifo.ned
//        (part of Fifo2 - an OMNeT++ demo simulation)
//-------------------------------------------------------------


// FF2PacketFifo --
//
// single-server queue
//
simple FF2PacketFifo
    parameters:
        service_time : numeric;
    gates:
        in: in;
        out: out;
endsimple


// FF2PacketFifo --
//
// single-server queues in another flavour
//
simple FF2BitFifo
    parameters:
        bits_per_sec : numeric;
    gates:
        in: in;
        out: out;
endsimple


