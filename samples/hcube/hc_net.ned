//--------------------------------------------------
// File: hc_net.ned
// 
// Definition of the hypercube network
// 
// Author: Andras Varga (TU Budapest)
// 
//--------------------------------------------------


import  
    "hc_rte.ned", 
    "gensink.ned", 
    "hcube.ned"; 


// HypercubeNode --
// 
// Building block of the delection routing hypercube. The interface of 
// this module conforms to the Node interface defined in hcube.ned.
// 
module HypercubeNode 
    parameters: 
        address : numeric, 
        dim : numeric; 
    gates: 
        in: in[]; 
        out: out[]; 
    submodules: 
        gen: Generator; 
            parameters: 
                num_stations = 2^dim, 
                address = address, 
                ia_time = input; 
        sink: Sink; 
            parameters: 
                address = address; 
        rte: HypercubeRte; 
            parameters: 
                address = address, 
                dim = dim; 
            gatesizes: 
                in[dim], 
                out[dim]; 
    connections: 
        gen.out --> rte.from_gen; 
        sink.in <-- rte.to_sink; 
        for i=0..dim-1 do 
            in[i] --> rte.in[i]; 
            out[i] <-- rte.out[i]; 
        endfor; 
endmodule 


// hypercube --
// 
// The actual hypercube network, instantiated as from the Hypercube
// topology template, substituting HypercubeNode for Node. 
// 
network hypercube : Hypercube 
    parameters: 
        dim = input(2,"Dimension (1,2,3,4,...)"), 
        nodetype = "HypercubeNode"; 
endnetwork 


