//-------------------------------------------------------------
// File: tknmods.ned
// 
// Declaration of simple module types
// 
// Authors: Gabor Lencse, Andras Varga (TU Budapest)
// Based on the code by:
//          Maurits Andr‚, George van Montfort,
//          Gerard van de Weerd (TU Delft)
//-------------------------------------------------------------


// TokenRingMAC --
// 
// implements the Token Ring protocol
// 
simple TokenRingMAC 
    parameters: 
        data_rate : numeric, 
        THT : numeric; 
    gates: 
        in: phy_in; 
        in: from_gen; 
        out: phy_out; 
        out: to_sink; 
endsimple 


// Generator --
// 
// generates data packets for MAC
// 
simple Generator 
    parameters: 
        num_messages : numeric, 
        num_stations : numeric const, 
        ia_time : numeric; 
    gates: 
        out: out; 
endsimple 


// Sink --
// 
// destroys the packets and creates statistics
// 
simple Sink 
    gates: 
        in: in; 
endsimple 


