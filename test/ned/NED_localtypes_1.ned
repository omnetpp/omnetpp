%description:
Test that the local Node type takes precedence over the global one.

%file: test.ned

module Node {  // the global type 
} 

network Test {
  types:
    module Node {  // the local type
    }
  submodules:
    dump: Dump;
    n: Node;  
}

%exitcode: 0

%inifile: test.ini
[General]
network = Test
cmdenv-express-mode = false
ned-path=.;../../lib

%contains: stdout
module NED_gatesize_1_network
module NED_gatesize_1_network.s
 