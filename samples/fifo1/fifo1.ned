//-------------------------------------------------------------
// file: fifo1.ned
//        (part of Fifo1 - an OMNeT++ demo simulation)
//-------------------------------------------------------------


// FF1PacketFifo --
//
// single-server queue, same service rate for each packet
//
simple FF1PacketFifo
    parameters:
        service_time : numeric;
    gates:
        in: in;
        out: out;
endsimple


// FF1BitFifo --
//
// single-server queue, service rate based on message length
//
simple FF1BitFifo
    parameters:
        bits_per_sec : numeric;
    gates:
        in: in;
        out: out;
endsimple



