//-------------------------------------------------------------
// File: gensink.ned
//
// Declaration of simple module types
//
// Author: Andras Varga (TU Budapest)
//-------------------------------------------------------------


// HCGenerator --
//
// generates data packets
//
simple HCGenerator
    parameters:
        num_stations : numeric,
        address : numeric,
        ia_time : numeric;
    gates:
        out: out;
endsimple


// HCSink --
//
// destroys the packets and creates statistics
//
simple HCSink
    parameters:
        address : numeric;
    gates:
        in: in;
endsimple


