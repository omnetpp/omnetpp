//--------------------------------------------------
// File: hcube.ned
//
// The Hypercube topology template
//
// Author: Andras Varga (TU Budapest)
//
//--------------------------------------------------

//----------
// When the user creates an actual hypercube, he substitutes the name
// of an existing module type (e.g. HypercubeNode) for the nodetype
// parameter. The module type implements the algorithm the user wants
// to simulate and it must have the same gates that the Node type has.
//
// The topology template code can be used through file inclusion.
// See hc_net.ned
//----------

simple Node
  parameters:
     address: numeric,
     dim: numeric;
  gates:
     out:  out[];
     in:   in[];
endsimple

module Hypercube
  parameters:
    dim: numeric const,
    nodetype: string;
  submodules:
    node: nodetype[2^dim] like Node
      parameters:
        address = index, // module's position in (2^dim)-size vector
        dim = dim;
      gatesizes:
        out[dim], in[dim];
  connections:
    for i=0..2^dim-1, j=0..dim-1 do
      node[i].out[j] --> node[i # (1<<j)].in[j];  // # is bitwise xor
    endfor;
endmodule
