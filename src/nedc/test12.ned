//
// Test 12: submodule-to-processor mapping for distributed execution
//

simple computer_def
        parameters:  memory_mb,
                     disk_mb;
        gates:       in:  serial_in[];
                     out: serial_out[];
endsimple

module network_def
        machines: node1, node2;
        submodules:
                computer1: computer_def[2]
                    on if index==0: node1;
                    on if index==1: node2;
                    parameters: memory_mb = 16,
                                disk_mb = 650;
                    gatesizes:  serial_in[2],
                                serial_out[2];
                computer2: computer_def
                    on: node2;
                    parameters: memory_mb = 24,
                                disk_mb = 1024;
                    gatesizes:  serial_in[4],
                                serial_out[4];
       connections:
                for i = 0..1, j = 0..1 do
                   computer1[j].serial_out[i] --> computer2.serial_in[i+2*j];
                   computer1[j].serial_in[i] <-- computer2.serial_out[i+2*j];
                endfor;
endmodule

network sample_network:  network_def
        on: phoenix, balu;
endnetwork

