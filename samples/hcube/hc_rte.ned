//-------------------------------------------------------------
// File: hc_rte.ned
// 
// Declaration of HypercubeRte simple module type
// 
// Author: Andras Varga (TU Budapest)
//-------------------------------------------------------------


// HypercubeRte --
// 
// implements deflection routing in a hypercube network
// 
simple HypercubeRte 
    parameters: 
        address : numeric, 
        dim : numeric; 
    gates: 
        in: in[]; 
        in: from_gen; 
        out: out[]; 
        out: to_sink; 
endsimple 


