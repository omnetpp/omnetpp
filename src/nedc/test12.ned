//
// Test 12: submodule-to-processor mapping for distributed execution I.
//

simple Computer
endsimple

module Network
        machines:
                node1, node2;
        submodules:
                computer1: Computer
                    on: node1;
                computer2: Computer
                    on: node2;
endmodule

network sample_network:  Network
        on:
                machine1, machine2;
endnetwork

