%description:
Test that the local Node type takes precedence over the global one.

%file: {}.ned

module {Node} {  // the global type 
} 

network {network} {
  types:
    module {Node} {  // the local type
    }
  submodules:
    dump: Dump;
    n: {Node};  
}

%exitcode: 0

%contains: stdout
module NED_gatesize_1_network
module NED_gatesize_1_network.s
 