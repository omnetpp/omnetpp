module Computer 
    parameters: 
        address : string; 
    gates: 
        in: in; 
        out: out; 
    submodules: 
        MAC: FDDI_MAC4Sniffer; 
            on: 
                host; 
            parameters: 
                StationID = StationID, 
                address = address; 
            display: "p=112,117;b=40,24,rect";
        gen: FDDI_Generator4Sniffer; 
            on: 
                host; 
            parameters: 
                StationID = StationID, 
                address = address; 
            display: "p=140,58;b=40,24,rect";
        monitor: FDDI_Monitor; 
            on: 
                host; 
                      
            display: "p=79,57;b=40,24,rect";
    connections: 
        in --> MAC.in; 
        MAC.out --> out; 
        MAC.to_LLC --> monitor.in; 
        gen.out --> MAC.from_LLC; 
                                  
endmodule 

module Subnet 
    submodules: 
        ethernet: Ethernet; 
            display: "p=165,139;b=274,6,rect;o=#8c958f,#737d76,";
        router: Computer; 
            display: "p=70,200;i=router";
        workstation1: Computer; 
            display: "p=75,74;i=pc";
        workstation2: Computer; 
            display: "p=143,203;i=pc";
        workstation3: Computer; 
            display: "p=176,54;i=pc";
        workstation4: Computer; 
            display: "p=238,193;i=pc";
        workstation5: Computer; 
            display: "p=261,80;i=pc";
    connections: 
        workstation1.out --> ethernet.in; 
        ethernet.out --> workstation1.in; 
        workstation3.out --> ethernet.in; 
        ethernet.out --> workstation3.in; 
        workstation5.out --> ethernet.in; 
        ethernet.out --> workstation5.in; 
        ethernet.out --> workstation2.in; 
        workstation2.out --> ethernet.in; 
        workstation4.out --> ethernet.in; 
        ethernet.out --> workstation4.in; 
        router.out --> ethernet.in; 
        ethernet.out --> router.in; 
        router.out --> in; 
        out --> router.in; 
endmodule 


module Network 
    submodules: 
        node1: Subnet; 
            display: "p=56,52;i=cloud_s";
        node2: Subnet; 
            display: "p=136,240;i=cloud_s";
        node3: Subnet; 
            display: "p=231,140;i=cloud_s";
        node4: Subnet; 
            display: "p=284,233;i=cloud_s";
        node5: Subnet; 
            display: "p=168,52;i=cloud_s";
        node6: Subnet; 
            display: "p=43,147;i=cloud_s";
        node7: Subnet; 
            display: "p=104,168;i=cloud_s";
        node8: Subnet; 
            display: "p=274,56;i=cloud_s";
    connections: 
        node1.out --> node5.in; 
        node7.out --> node1.in; 
        node1.out --> node7.in; 
        node5.out --> node1.in; 
        node5.out --> node3.in; 
        node3.out --> node5.in; 
        node3.out --> node4.in; 
        node4.out --> node3.in; 
        node7.out --> node2.in; 
        node2.out --> node7.in; 
        node7.out --> node6.in; 
        node6.out --> node7.in; 
        node8.out --> node3.in; 
        node3.out --> node8.in; 
        node4.out --> node2.in; 
        node2.out --> node4.in; 
        node7.out --> node3.in; 
        node3.out --> node7.in; 
        node5.out --> node8.in; 
        node8.out --> node5.in; 
    display: "p=10,10;b=324,265,rect";
endmodule 


