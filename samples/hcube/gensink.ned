//-------------------------------------------------------------
// File: gensink.ned
//
// Declaration of simple module types
//
// Author: Andras Varga (TU Budapest)
//-------------------------------------------------------------

// Generator: generates data packets
simple Generator
        parameters:
                num_stations: numeric,
                address: numeric,
                ia_time: numeric;
        gates:
                out: out;
endsimple

// Sink: destroys the packets and creates statistics
simple Sink
        parameters:
                address: numeric;
        gates:
                in: in;
endsimple
