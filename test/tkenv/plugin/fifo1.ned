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
        serviceRate: numeric;
    gates:
        in: in[];
        out: out;
endsimple


